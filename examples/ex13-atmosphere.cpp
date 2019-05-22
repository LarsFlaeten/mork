#include <iostream>
#include <stdexcept>
#include <vector>
#include <sstream>
#include <set>
#include <cxxopts.hpp>
#include <random>

#include "mork/ui/GlfwWindow.h"
#include "mork/core/Log.h"
#include "mork/render/Font.h"
#include "mork/scene/Scene.h"
#include "mork/scene/SceneNode.h"
#include "mork/render/Light.h"
#include "mork/math/quat.h"
#include "mork/render/Mesh.h"
#include "mork/render/Material.h"
#include "mork/util/ModelImporter.h"
#include "mork/render/Framebuffer.h"
#include "mork/core/stb_image.h"
#include "mork/util/MeshUtil.h"

#include "mork/util/BBoxDrawer.h"
#include "mork/resource/ResourceManager.h"
#include "mork/resource/ResourceFactory.h"

#include "mork/atmosphere/model.h"

using namespace std;
using namespace mork;

constexpr double kPi = 3.1415926;
constexpr double kSunAngularRadius = 0.00935 / 2.0;
constexpr double kSunSolidAngle = kPi * kSunAngularRadius * kSunAngularRadius;
constexpr double kLengthUnitInMeters = 1000.0;

const char kVertexShader[] = R"(
    uniform mat4 model_from_view;
    uniform mat4 view_from_clip;
    layout(location = 0) in vec4 vertex;
    out vec3 view_ray;
    void main() {
      view_ray =
          (model_from_view * vec4((view_from_clip * vertex).xyz, 0.0)).xyz;
      gl_Position = vertex;
    })";

//#include "mork/atmosphere/demo/demo.glsl.inc"



class TextBox {
    public:
        TextBox(int width, int height) : 
            fb(width, height, false), 
            dirty(true), 
            text(""),
            ortho(mork::mat4f::orthoProjection(width, 0.0f, height, 0.0f, -1.0f, 1.0f)) 
        {
            // Set background as initially transparent
            fb.setClearColor(mork::vec4f(0.0f, 0.0f, 0.0f, 0.0f));
        }
        
        void setText(const std::string& _text) {
            if(_text.compare(text) != 0) {
                text = _text;
                dirty = true;
            }
        }

        void drawToBuffer(mork::Font& font) {
            if(dirty) {
                fb.bind();
                fb.clear();
                auto s = fb.getSize();
                font.drawText(text, 5, s.y-font.getYMax(18)-5, 18, mork::vec3f(1.0, 1.0, 1.0), ortho);
                dirty = false;       

            }
        }

        void draw(mork::Font& font) {
                auto s = fb.getSize();
                font.drawText(text, 5, s.y-font.getYMax(18)-5, 18, mork::vec3f(1.0, 1.0, 1.0), ortho);
                dirty = false;       
        }


        const mork::Texture<2>& getColorBuffer() const {
            return fb.getColorBuffer();
        }

    private:
        bool dirty;
        std::string text;
        mork::Framebuffer fb;
        mork::mat4f ortho; 

};


using VN = mork::vertex_pos_norm_uv;

class App : public mork::GlfwWindow {
    private:
        enum Luminance {
            // Render the spectral radiance at kLambdaR, kLambdaG, kLambdaB.
            NONE,
            // Render the sRGB luminance, using an approximate (on the fly) conversion
            // from 3 spectral radiance values only (see section 14.3 in <a href=
            // "https://arxiv.org/pdf/1612.04336.pdf">A Qualitative and Quantitative
            //  Evaluation of 8 Clear Sky Models</a>).
            APPROXIMATE,
            // Render the sRGB luminance, precomputed from 15 spectral radiance values
            // (see section 4.4 in <a href=
            // "http://www.oskee.wz.cz/stranka/uploads/SCCG10ElekKmoch.pdf">Real-time
            //  Spectral Scattering in Large-scale Natural Participating Media</a>).::
            PRECOMPUTED
        };

    public:
    
        App(mork::ResourceManager& _manager)
            :   mork::GlfwWindow(mork::ResourceFactory<mork::Window::Parameters>::getInstance().create(_manager,"window1")),
                manager(_manager),
                progs(mork::ResourceFactory<mork::ProgramPool>::getInstance().create(manager,"programPool1")),
                font(mork::Font::createFont("resources/fonts/LiberationSans-Regular.ttf", 48)),
                textBox(800, 600),
                fsQuad(mork::MeshHelper<vertex_pos4>::PLANE()),
                showHelp(false),
                use_constant_solar_spectrum_(false),
                use_ozone_(true),
                use_combined_textures_(true),
                use_half_precision_(true),
                use_luminance_(NONE),
                do_white_balance_(false),
                view_distance_meters_(9000.0),
                view_zenith_angle_radians_(1.47),
                view_azimuth_angle_radians_(-0.1),
                sun_zenith_angle_radians_(1.3),
                sun_azimuth_angle_radians_(2.9),
                is_ctrl_key_pressed_(false),
                exposure_(10.0) 
             
        {

            mork::GlfwWindow::waitForVSync(false);
            InitModel();
        }

        ~App() {
        }

/*
<p>The "real" initialization work, which is specific to our atmosphere model,
is done in the following method. It starts with the creation of an atmosphere
<code>Model</code> instance, with parameters corresponding to the Earth
atmosphere:
*/
        void InitModel() {
            mork::debug_logger("Init atmospheric model...");
			// Values from "Reference Solar Spectral Irradiance: ASTM G-173", ETR column
			// (see http://rredc.nrel.gov/solar/spectra/am1.5/ASTMG173/ASTMG173.html),
			// summed and averaged in each bin (e.g. the value for 360nm is the average
			// of the ASTM G-173 values for all wavelengths between 360 and 370nm).
			// Values in W.m^-2.
			constexpr int kLambdaMin = 360;
			constexpr int kLambdaMax = 830;
			constexpr double kSolarIrradiance[48] = {
				1.11776, 1.14259, 1.01249, 1.14716, 1.72765, 1.73054, 1.6887, 1.61253,
				1.91198, 2.03474, 2.02042, 2.02212, 1.93377, 1.95809, 1.91686, 1.8298,
				1.8685, 1.8931, 1.85149, 1.8504, 1.8341, 1.8345, 1.8147, 1.78158, 1.7533,
				1.6965, 1.68194, 1.64654, 1.6048, 1.52143, 1.55622, 1.5113, 1.474, 1.4482,
				1.41018, 1.36775, 1.34188, 1.31429, 1.28303, 1.26758, 1.2367, 1.2082,
				1.18737, 1.14683, 1.12362, 1.1058, 1.07124, 1.04992
			};
			// Values from http://www.iup.uni-bremen.de/gruppen/molspec/databases/
			// referencespectra/o3spectra2011/index.html for 233K, summed and averaged in
			// each bin (e.g. the value for 360nm is the average of the original values
			// for all wavelengths between 360 and 370nm). Values in m^2.
			constexpr double kOzoneCrossSection[48] = {
				1.18e-27, 2.182e-28, 2.818e-28, 6.636e-28, 1.527e-27, 2.763e-27, 5.52e-27,
				8.451e-27, 1.582e-26, 2.316e-26, 3.669e-26, 4.924e-26, 7.752e-26, 9.016e-26,
				1.48e-25, 1.602e-25, 2.139e-25, 2.755e-25, 3.091e-25, 3.5e-25, 4.266e-25,
				4.672e-25, 4.398e-25, 4.701e-25, 5.019e-25, 4.305e-25, 3.74e-25, 3.215e-25,
				2.662e-25, 2.238e-25, 1.852e-25, 1.473e-25, 1.209e-25, 9.423e-26, 7.455e-26,
				6.566e-26, 5.105e-26, 4.15e-26, 4.228e-26, 3.237e-26, 2.451e-26, 2.801e-26,
				2.534e-26, 1.624e-26, 1.465e-26, 2.078e-26, 1.383e-26, 7.105e-27
			};
			// From https://en.wikipedia.org/wiki/Dobson_unit, in molecules.m^-2.
			constexpr double kDobsonUnit = 2.687e20;
			// Maximum number density of ozone molecules, in m^-3 (computed so at to get
			// 300 Dobson units of ozone - for this we divide 300 DU by the integral of
			// the ozone density profile defined below, which is equal to 15km).
			constexpr double kMaxOzoneNumberDensity = 300.0 * kDobsonUnit / 15000.0;
			// Wavelength independent solar irradiance "spectrum" (not physically
			// realistic, but was used in the original implementation).
			constexpr double kConstantSolarIrradiance = 1.5;
			constexpr double kBottomRadius = 6360000.0;
			constexpr double kTopRadius = 6420000.0;
			constexpr double kRayleigh = 1.24062e-6;
			constexpr double kRayleighScaleHeight = 8000.0;
			constexpr double kMieScaleHeight = 1200.0;
			constexpr double kMieAngstromAlpha = 0.0;
			constexpr double kMieAngstromBeta = 5.328e-3;
			constexpr double kMieSingleScatteringAlbedo = 0.9;
			constexpr double kMiePhaseFunctionG = 0.8;
			constexpr double kGroundAlbedo = 0.1;
			const double max_sun_zenith_angle =
			  (use_half_precision_ ? 102.0 : 120.0) / 180.0 * kPi;

			atmosphere::DensityProfileLayer
			  rayleigh_layer(0.0, 1.0, -1.0 / kRayleighScaleHeight, 0.0, 0.0);
			atmosphere::DensityProfileLayer mie_layer(0.0, 1.0, -1.0 / kMieScaleHeight, 0.0, 0.0);
			// Density profile increasing linearly from 0 to 1 between 10 and 25km, and
			// decreasing linearly from 1 to 0 between 25 and 40km. This is an approximate
			// profile from http://www.kln.ac.lk/science/Chemistry/Teaching_Resources/
			// Documents/Introduction%20to%20atmospheric%20chemistry.pdf (page 10).
			std::vector<atmosphere::DensityProfileLayer> ozone_density;
			ozone_density.push_back(
			  atmosphere::DensityProfileLayer(25000.0, 0.0, 0.0, 1.0 / 15000.0, -2.0 / 3.0));
			ozone_density.push_back(
			  atmosphere::DensityProfileLayer(0.0, 0.0, 0.0, -1.0 / 15000.0, 8.0 / 3.0));

			std::vector<double> wavelengths;
			std::vector<double> solar_irradiance;
			std::vector<double> rayleigh_scattering;
			std::vector<double> mie_scattering;
			std::vector<double> mie_extinction;
			std::vector<double> absorption_extinction;
			std::vector<double> ground_albedo;
			for (int l = kLambdaMin; l <= kLambdaMax; l += 10) {
				double lambda = static_cast<double>(l) * 1e-3;  // micro-meters
				double mie =
					kMieAngstromBeta / kMieScaleHeight * pow(lambda, -kMieAngstromAlpha);
				wavelengths.push_back(l);
				if (use_constant_solar_spectrum_) {
					solar_irradiance.push_back(kConstantSolarIrradiance);
				} else {
					solar_irradiance.push_back(kSolarIrradiance[(l - kLambdaMin) / 10]);
				}
				rayleigh_scattering.push_back(kRayleigh * pow(lambda, -4));
				mie_scattering.push_back(mie * kMieSingleScatteringAlbedo);
				mie_extinction.push_back(mie);
				absorption_extinction.push_back(use_ozone_ ?
					kMaxOzoneNumberDensity * kOzoneCrossSection[(l - kLambdaMin) / 10] :
					0.0);
				ground_albedo.push_back(kGroundAlbedo);
			} 
            
            model_.reset(new atmosphere::Model(wavelengths, solar_irradiance, kSunAngularRadius,
                kBottomRadius, kTopRadius, {rayleigh_layer}, rayleigh_scattering,
                {mie_layer}, mie_scattering, mie_extinction, kMiePhaseFunctionG,
                ozone_density, absorption_extinction, ground_albedo, max_sun_zenith_angle,
                kLengthUnitInMeters, use_luminance_ == PRECOMPUTED ? 15 : 3,
                use_combined_textures_, use_half_precision_));
            
            model_->Init();
            /*
            <p>Then, it creates and compiles the vertex and fragment shaders used to render
            our demo scene, and link them with the <code>Model</code>'s atmosphere shader
            to get the final scene rendering program:
            */
            mork::Shader vs(330, kVertexShader, mork::Shader::Type::VERTEX, "");


            std::ifstream is("shaders/atmo_demo.glsl");
            std::stringstream ss;
            ss << is.rdbuf();
            const std::string fragment_shader_str =
                std::string(use_luminance_ != NONE ? "#define USE_LUMINANCE\n" : "") +
                "const float kLengthUnitInMeters = " +
                std::to_string(kLengthUnitInMeters) + ";\n" +
                ss.str();
     
            mork::Shader fs(330, fragment_shader_str, mork::Shader::Type::FRAGMENT, "");

            // Attach api functions from atmosphere model:
            program.buildProgram({vs, fs, model_->shader()}); 
            /*
            <p>Finally, it sets the uniforms of this program that can be set once and for
            all (in our case this includes the <code>Model</code>'s texture uniforms,
            because our demo app does not have any texture of its own):
            */

            program.use();
            model_->SetProgramUniforms(program, 0, 1, 2, 3);
            double white_point_r = 1.0;
            double white_point_g = 1.0;
            double white_point_b = 1.0;
            if (do_white_balance_) {
                atmosphere::Model::ConvertSpectrumToLinearSrgb(wavelengths, solar_irradiance,
                    &white_point_r, &white_point_g, &white_point_b);
                double white_point = (white_point_r + white_point_g + white_point_b) / 3.0;
                white_point_r /= white_point;
                white_point_g /= white_point;
                white_point_b /= white_point;
            }
            program.getUniform("white_point").set(mork::vec3f(white_point_r, white_point_g, white_point_b));
            program.getUniform("earth_center").set(mork::vec3f(0.0, 0.0, -kBottomRadius / kLengthUnitInMeters));
            program.getUniform("sun_size").set(mork::vec2f(tan(kSunAngularRadius), cos(kSunAngularRadius)));

            // This sets 'view_from_clip', which only depends on the window size.
            reshape(this->getWidth(), this->getHeight());

        }

        void reloadResources() {
        }

        virtual void redisplay(double t, double dt) {
           
            // Framebuffer ops 
            mork::Framebuffer::getDefault().bind();
            mork::Framebuffer::getDefault().clear();

            // State group:
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     



            double timeValue = timer.getTime();
         
            // Update sceme:
            // Adjust camera:
            // Unit vectors of the camera frame, expressed in world space.
            float cos_z = cos(view_zenith_angle_radians_);
            float sin_z = sin(view_zenith_angle_radians_);
            float cos_a = cos(view_azimuth_angle_radians_);
            float sin_a = sin(view_azimuth_angle_radians_);
            float ux[3] = { -sin_a, cos_a, 0.0 };
            float uy[3] = { -cos_z * cos_a, -cos_z * sin_a, sin_z };
            float uz[3] = { sin_z * cos_a, sin_z * sin_a, cos_z };
            float l = view_distance_meters_ / kLengthUnitInMeters;

            // Transform matrix from camera frame to world space (i.e. the inverse of a
            // GL_MODELVIEW matrix).
            mat4f model_from_view = {
                ux[0], uy[0], uz[0], uz[0] * l,
                ux[1], uy[1], uz[1], uz[1] * l,
                ux[2], uy[2], uz[2], uz[2] * l,
                0.0, 0.0, 0.0, 1.0
            };
           
            program.use(); 
            program.getUniform("camera").set(mork::vec3f(
                  model_from_view[0][3],
                  model_from_view[1][3],
                  model_from_view[2][3]));
            program.getUniform("exposure").set(
                  use_luminance_ != NONE ? exposure_ * 1e-5 : exposure_);
            program.getUniform("model_from_view").set(
                  model_from_view);
            program.getUniform("sun_direction").set(mork::vec3f(
                  cos(sun_azimuth_angle_radians_) * sin(sun_zenith_angle_radians_),
                  sin(sun_azimuth_angle_radians_) * sin(sun_zenith_angle_radians_),
                  cos(sun_zenith_angle_radians_)));

            fsQuad.draw();

     
            // Draw 2D Text 
            
            // State group, only diff is shown
            if(showHelp) {
                glDisable(GL_DEPTH_TEST);
                glDisable(GL_BLEND);
                //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

               
                mork::mat4f ortho = mork::mat4f::orthoProjection(this->getWidth(), 0.0f, this->getHeight(), 0.0f, -1.0f, 1.0f);

                std::stringstream info;
                info << "Multiline text:\n";
                info << "FPS: " << this->getFps() << ", frametime: " << this->getFrameTime() << "s\n";
                info << "Mouse:\n"
                 << " drag, CTRL+drag, wheel: view and sun directions\n"
                 << "Keys:\n"
                 << " h: help\n"
                 << " s: solar spectrum (currently: "
                 << (use_constant_solar_spectrum_ ? "constant" : "realistic") << ")\n"
                 << " o: ozone (currently: " << (use_ozone_ ? "on" : "off") << ")\n"
                 << " t: combine textures (currently: "
                 << (use_combined_textures_ ? "on" : "off") << ")\n"
                 << " p: half precision (currently: "
                 << (use_half_precision_ ? "on" : "off") << ")\n"
                 << " l: use luminance (currently: "
                 << (use_luminance_ == PRECOMPUTED ? "precomputed" :
                     (use_luminance_ == APPROXIMATE ? "approximate" : "off")) << ")\n"
                 << " w: white balance (currently: "
                 << (do_white_balance_ ? "on" : "off") << ")\n"
                 << " up/down: increase/decrease exposure (" << exposure_ << ")\n"
                 << " 1-9: predefined views\n";
                info << " Press [ESC] to quit\n"; 
                info << " Model/input data:\n"
                 << " View distance (m): " << view_distance_meters_ << "\n"
                 << " CTRL: " << (is_ctrl_key_pressed_ ? "yes" : "no") << "\n"
 				 << " Sun zenith angle [deg]:   " << degrees(sun_zenith_angle_radians_) << "\n"
                 << " Sun azimuth angle [deg]:  " << degrees(sun_azimuth_angle_radians_) << "\n"
  				 << " View zenith angle [deg]:  " << degrees(view_zenith_angle_radians_) << "\n"
                 << " View azimuth angle [deg]: " << degrees(view_azimuth_angle_radians_) << "\n";

    			info << " Keys: ";
                for(auto c: keys)
                    info << c << "[" << (int)c << "], ";
                        

                textBox.setText(info.str());
                textBox.drawToBuffer(font);

                // State changes
                glEnable(GL_BLEND); 
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                mork::Framebuffer::getDefault().bind();
               
                mork::Program& quadProg = progs.at("quadProg");
                quadProg.use(); 
                quadProg.bindTexture(textBox.getColorBuffer(), "tex", 0);
                fsQuad.draw();
            }

            GlfwWindow::redisplay(t, dt);
            
        }

        virtual void reshape(int width, int height)
        {
            // Resizes default framebuffer (will also sort out glViewport
            mork::Framebuffer::getDefault().setSize(mork::vec2i(width, height));
            GlfwWindow::reshape(width, height);
            const float kFovY = 20.0 / 180.0 * kPi;
            const float kTanFovY = tan(kFovY / 2.0);
            float aspect_ratio = static_cast<float>(width) / height;

            // Transform matrix from clip space to camera space (i.e. the inverse of a
            // GL_PROJECTION matrix).
            float view_from_clip[16] = {
                kTanFovY * aspect_ratio, 0.0, 0.0, 0.0,
                0.0, kTanFovY, 0.0, 0.0,
                0.0, 0.0, 0.0, -1.0,
                0.0, 0.0, 1.0, 1.0
            };
            auto program_ = program.getProgramId();
            glUniformMatrix4fv(glGetUniformLocation(program_, "view_from_clip"), 1, true,
                view_from_clip);

            idle(false);
        }

        virtual bool specialKey(key k, modifier m, int x, int y)
        {
            bool handled = false;
            switch (k) {
                case mork::EventHandler::key::KEY_ESCAPE:
                    shouldClose();             
                    handled =  true;
                    break;
                case mork::EventHandler::key::KEY_LEFT_CONTROL:
                    is_ctrl_key_pressed_ = true;
                    handled = true;
                    break;
                case mork::EventHandler::key::KEY_RIGHT_CONTROL:
                    is_ctrl_key_pressed_ = true;
                    handled = true;
                    break;
               default:
                   break;
            }
            return false;
        }
        
        virtual bool specialKeyReleased(key k, modifier m, int x, int y)
        {
            bool handled = false;
            switch (k) {
                case mork::EventHandler::key::KEY_ESCAPE:
                    shouldClose();             
                    handled =  true;
                    break;
                case mork::EventHandler::key::KEY_UP:
                    exposure_ *= 1.1;
                    handled = true;
                    break;
                case mork::EventHandler::key::KEY_DOWN:
                    exposure_ /= 1.1;
                    handled = true;
                    break;
                case mork::EventHandler::key::KEY_LEFT_CONTROL:
                    is_ctrl_key_pressed_ = false;
                    handled = true;
                    break;
                case mork::EventHandler::key::KEY_RIGHT_CONTROL:
                    is_ctrl_key_pressed_ = false;
                    handled = true;
                    break;
               default:
                    break;
            }
            return handled;
        }


        virtual bool keyTyped(unsigned char c, modifier m, int x, int y)
        {
            auto key = c;
            keys.insert(c);
              if (key == 'S') {
                use_constant_solar_spectrum_ = !use_constant_solar_spectrum_;
              } else if (key == 'O') {
                use_ozone_ = !use_ozone_;
              } else if (key == 'T') {
                use_combined_textures_ = !use_combined_textures_;
              } else if (key == 'P') {
                use_half_precision_ = !use_half_precision_;
              } else if (key == 'L') {
                switch (use_luminance_) {
                  case NONE: use_luminance_ = APPROXIMATE; break;
                  case APPROXIMATE: use_luminance_ = PRECOMPUTED; break;
                  case PRECOMPUTED: use_luminance_ = NONE; break;
                }
              } else if (key == 'W') {
                do_white_balance_ = !do_white_balance_;
             } else if (key == '1') {
                SetView(9000.0, 1.47, 0.0, 1.3, 3.0, 10.0);
              } else if (key == '2') {
                SetView(9000.0, 1.47, 0.0, 1.564, -3.0, 10.0);
              } else if (key == '3') {
                SetView(7000.0, 1.57, 0.0, 1.54, -2.96, 10.0);
              } else if (key == '4') {
                SetView(7000.0, 1.57, 0.0, 1.328, -3.044, 10.0);
              } else if (key == '5') {
                SetView(9000.0, 1.39, 0.0, 1.2, 0.7, 10.0);
              } else if (key == '6') {
                SetView(9000.0, 1.5, 0.0, 1.628, 1.05, 200.0);
              } else if (key == '7') {
                SetView(7000.0, 1.43, 0.0, 1.57, 1.34, 40.0);
              } else if (key == '8') {
                SetView(2.7e6, 0.81, 0.0, 1.57, 2.0, 10.0);
              } else if (key == '9') {
                SetView(1.2e7, 0.0, 0.0, 0.93, -2.0, 10.0);
              }
              if (key == 'S' || key == 'O' || key == 'T' || key == 'P' || key == 'L' ||
                  key == 'W') {
                InitModel();
              }

            if(keys.count('H'))
                showHelp = !showHelp;
            return true;
        }

        virtual bool keyReleased(unsigned char c, modifier m, int x, int y) {
            keys.erase(c);

            return true;
        }

        // Mouse motion while clicked
        virtual bool mouseMotion(int x, int y) {
  			constexpr double kScale = 500.0;
  			if (is_ctrl_key_pressed_) {
    			sun_zenith_angle_radians_ -= (previous_mouse_y_ - y) / kScale;
    			sun_zenith_angle_radians_ =
        			std::max(0.0, std::min(kPi, sun_zenith_angle_radians_));
    			sun_azimuth_angle_radians_ += (previous_mouse_x_ - x) / kScale;
  			} else {
    			view_zenith_angle_radians_ += (previous_mouse_y_ - y) / kScale;
    			view_zenith_angle_radians_ =
        			std::max(0.0, std::min(kPi / 2.0, view_zenith_angle_radians_));
    			view_azimuth_angle_radians_ += (previous_mouse_x_ - x) / kScale;
  			}
  			previous_mouse_x_ = x;
  			previous_mouse_y_ = y;
            return true;
        }

        // Mouse motion while no buttons clicked
        virtual bool mousePassiveMotion(int x, int y) {
            return false;
        }
    
        virtual bool mouseClick(button b, state s, modifier m, int x, int y) {
            previous_mouse_x_ = x;
            previous_mouse_y_ = y;

        }

        virtual bool mouseWheel(wheel b, modifier m, int x, int y) {
            
            if (b==mork::EventHandler::WHEEL_UP ) {
                view_distance_meters_ *= 1.05;
            } else {
                view_distance_meters_ /= 1.05;
            }
        }


        virtual void fpsUpdatedEvent() {
            std::stringstream os;
            os << this->getTitle() + " (FPS: " << this->getFps() << ")";
            this->setWindowTitle(os.str());

        }
        
        void SetView(double view_distance_meters,
            double view_zenith_angle_radians, double view_azimuth_angle_radians,
            double sun_zenith_angle_radians, double sun_azimuth_angle_radians,
            double exposure) {
            view_distance_meters_ = view_distance_meters;
            view_zenith_angle_radians_ = view_zenith_angle_radians;
            view_azimuth_angle_radians_ = view_azimuth_angle_radians;
            sun_zenith_angle_radians_ = sun_zenith_angle_radians;
            sun_azimuth_angle_radians_ = sun_azimuth_angle_radians;
            exposure_ = exposure;
        }


    private:
        bool showHelp;
     
        mork::ResourceManager& manager;


        mork::Timer timer;

        mork::Font font;

        mork::ProgramPool progs;


        std::set<char> keys;
     
        TextBox textBox;

        mork::Mesh<vertex_pos4>  fsQuad;

        std::unique_ptr<atmosphere::Model> model_;
        
        mork::Program program;
 
        int window_id_;


        bool use_constant_solar_spectrum_;
        bool use_ozone_;
        bool use_combined_textures_;
        bool use_half_precision_;
        Luminance use_luminance_;
        bool do_white_balance_;

        double view_distance_meters_;
        double view_zenith_angle_radians_;
        double view_azimuth_angle_radians_;
        double sun_zenith_angle_radians_;
        double sun_azimuth_angle_radians_;
        double exposure_;

        int previous_mouse_x_;
        int previous_mouse_y_;
        bool is_ctrl_key_pressed_;
     
    };


int main(int argc, char** argv) {

    mork::Timer timer;

    string exename(argv[0]);

    //string inputFile;
    //int     verbose = 0;
    cxxopts::Options options(argv[0], "Mork demo \n(c) 2019 Lars Flæten");
    options.add_options()
        ("h,help", "Print help")
        ;


    auto result = options.parse(argc, argv);

    if(result.count("help"))
    {
        cout << options.help({""}) << endl;
        return 0;
    }
    mork::info_logger("Starting client application..");   
    timer.start();
    mork::ResourceManager manager("ex13.json");

    App app(manager);

    app.start();
   
    timer.end();
    mork::info_logger("Client application shutdown.");   
    mork::info_logger("Took: ", timer.getDuration(), "s"); 

    return 0;
}


