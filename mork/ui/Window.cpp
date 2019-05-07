/*
 * Ork: a small object-oriented OpenGL Rendering Kernel.
 * Website : http://ork.gforge.inria.fr/
 * Copyright (c) 2008-2015 INRIA - LJK (CNRS - Grenoble University)
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, 
 * this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 * this list of conditions and the following disclaimer in the documentation 
 * and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its contributors 
 * may be used to endorse or promote products derived from this software without 
 * specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/*
 * Ork is distributed under the BSD3 Licence. 
 * For any assistance, feedback and remarks, you can check out the 
 * mailing list on the project page : 
 * http://ork.gforge.inria.fr/
 */
/*
 * Main authors: Eric Bruneton, Antoine Begault, Guillaume Piolat.
 */

#include "Window.h"
#include "mork/resource/ResourceFactory.h"

#include <string>

namespace mork
{

Window::Parameters::Parameters() :
    _name("Untitled"), _width(640), _height(480), _major(3), _minor(3),
    _debug(false), _alpha(false), _depth(false), _stencil(false), _multiSample(false)
{
}

std::string Window::Parameters::name() const
{
    return _name;
}

int Window::Parameters::width() const
{
    return _width;
}

int Window::Parameters::height() const
{
    return _height;
}

mork::vec2i Window::Parameters::getVersion() const
{
    return vec2i(_major, _minor);
}

bool Window::Parameters::debug() const
{
    return _debug;
}

bool Window::Parameters::alpha() const
{
    return _alpha;
}

bool Window::Parameters::depth() const
{
    return _depth;
}

bool Window::Parameters::stencil() const
{
    return _stencil;
}

bool Window::Parameters::multiSample() const
{
    return _multiSample;
}

Window::Parameters &Window::Parameters::name(const std::string name)
{
    _name = name;
    return *this;
}

Window::Parameters &Window::Parameters::size(int width, int height)
{
    _width = width;
    _height = height;
    return *this;
}

Window::Parameters &Window::Parameters::version(int major, int minor, bool debug)
{
    _major = major;
    _minor = minor;
    _debug = debug;
    return *this;
}

Window::Parameters &Window::Parameters::alpha(bool alpha)
{
    _alpha = alpha;
    return *this;
}

Window::Parameters &Window::Parameters::depth(bool depth)
{
    _depth = depth;
    return *this;
}

Window::Parameters &Window::Parameters::stencil(bool stencil)
{
    _stencil = stencil;
    return *this;
}

Window::Parameters &Window::Parameters::multiSample(bool multiSample)
{
    _multiSample = multiSample;
    return *this;
}

Window::Window(const Parameters &params) 
    : EventHandler(), windowTitle(params.name())
{
}

Window::~Window()
{
}

const std::string& Window::getTitle() const { return windowTitle; }


inline json parametersSchema = R"(
    {
        "$schema": "http://json-schema.org/draft-07/schema#",
        "title": "Window parameters schema",
        "type": "object",
        "description": "A window paraemeters object",
        "properties": {
            "name": { "type": "string" },
            "title": { "type": "string" },
            "version": { 
                "type": "object",
                "properties": {
                    "major": { "type": "integer" },
                    "minor": { "type": "integer" },
                    "debug": { "type": "boolean" }
                },
                "required": ["major", "minor", "debug"],
                "additionalProperties": false
            },
            "width": { "type": "integer" },
            "height": { "type": "integer" },
            "alpha": { "type": "boolean" },
            "depth": { "type": "boolean" },
            "stencil": { "type": "boolean" },
            "mulitSample": { "type": "boolean" }
        },
        "additionalProperties": false,
        "required": ["name"]
    }
    )"_json;


    class ParametersResource: public ResourceTemplate<Window::Parameters>
    {
		public:
		    ParametersResource(ResourceManager& manager, Resource& r) :
				ResourceTemplate<Window::Parameters>(parametersSchema), parameters()
			{
		        info_logger("Resource - Parameters");
                
                const json& js = r.getDescriptor();
                debug_logger("desc: " , js);
                validator.validate(js);

                // For default values, see Window::Parameters constructor
                if(js.count("title")) {
                    parameters.name(js["title"].get<std::string>());
                }

                if(js.count("width") || js.count("height")) {
                    if(!(js.count("width") && js.count("height")))
                    {
                        error_logger("Both width and height must be given");
                        throw std::invalid_argument(error_logger.last());
                    }    
                    parameters.size(js["width"].get<int>(), js["height"].get<int>());
                }

                if(js.count("version")) {
                    json ver = js["version"];
                    
                    auto maj = ver["major"].get<int>();
                    auto min = ver["minor"].get<int>();
                    auto debug = ver["debug"].get<bool>();
                    parameters.version(maj, min, debug);


                }                
                if(js.count("alpha")) {
                    parameters.alpha(js["alpha"].get<bool>());
                }

                if(js.count("depth")) {
                    parameters.depth(js["depth"].get<bool>());
                }

                if(js.count("stencil")) {
                    parameters.stencil(js["stencil"].get<bool>());
                }

                if(js.count("mulitSample")) {
                    parameters.multiSample(js["mulitSample"].get<bool>());
                }
               
                const auto& p = parameters;
                info_logger(
                        "Created window: \n\tsize: ", p.width(), " x ", p.height(),
                        "\n\tOpenGL version: ", p.getVersion().x, ".", p.getVersion().y,
                        "\n\tDebug context: ", p.debug(),
                        "\n\tAlpha: ", p.alpha(),
                        "\n\tDepth: ", p.depth(),
                        "\n\tStencil:", p.stencil(),
                        "\n\tMultiSample: ", p.multiSample());





		    }

            Window::Parameters releaseResource() {
				return std::move(parameters);

            }
		private:
            Window::Parameters parameters;			

    };

    inline std::string _parameters = "windowParameters";

    static ResourceFactory<Window::Parameters>::Type<_parameters, ParametersResource> ParametersType;


}
