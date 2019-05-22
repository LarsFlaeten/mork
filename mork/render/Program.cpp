#include "mork/glad/glad.h"
#include "mork/render/Program.h"
#include "mork/core/Log.h"
#include "mork/ui/GlfwWindow.h"
#include <cstring>
#include <vector>
#include <string>

#include "mork/resource/ResourceFactory.h"

namespace mork {

    Shader::Shader(int version, const std::string& src, Shader::Type type, const std::string& define = "") :
        _id(0), _type(type) 
    {
        if(src.empty()) {
            warn_logger("Empty source given to Shader constructor");
            return;
        }

        std::stringstream s;
        s << "#version " << version << " core\n";
        if(!define.empty())
            s << "#define " << define << "\n";
        s << src;

        buildShader(s.str());
    }

    Shader::~Shader() {
        if(GlfwWindow::isContextActive())
        {
            if(_id) {
                glDeleteShader(_id);
                _id = 0;
            }
        }   
    }
 
    Shader::Shader(Shader&& o) noexcept {
        _id = o._id;
        _type = o._type;
        o._id = 0;
    }
     
       
    Shader& Shader::operator=(Shader&& o) noexcept {
        if(GlfwWindow::isContextActive())
        {
            if(_id)
                glDeleteShader(_id);
        }   
        _id = o._id;
        _type = o._type;
        o._id = 0;
        return *this;
    }
    
    std::string   printLineNos(const std::string& s) {
        std::stringstream ss(s);
        std::stringstream  out;
        std::string line;
        int i = 0;


        while(std::getline(ss, line)) {
            // Line no;
            ++i;        
            out << i << ": " << line << "\n";
        } 

        return out.str();
    }

    Shader::Type Shader::getType() const {
        return _type;
    }
 
    int Shader::getId() const {
        return _id;
    }
 
    void Shader::buildShader(const std::string& src) {
        if(!GlfwWindow::isContextActive()) {
            error_logger("No context available when building shader, returning..");
            return;
        }
        if(_id!=0) {
            glDeleteShader(_id);
            _id = 0;
        }

        auto s = preProcess(src);
        
        const char* c_s = s.c_str();

        // build and compile our shader program
        // ------------------------------------
        try {
            switch(_type) {
                case(VERTEX):
                    mork::debug_logger("Compiling vertex shader");
                    _id = glCreateShader(GL_VERTEX_SHADER);
                    break;
                case(FRAGMENT):
                    mork::debug_logger("Compiling fragment shader");
                    _id = glCreateShader(GL_FRAGMENT_SHADER);
                    break;
                case(GEOMETRY):
                    mork::debug_logger("Compiling geometry shader");
                    _id = glCreateShader(GL_GEOMETRY_SHADER);
                    break;
                default:
                    throw std::runtime_error("Not implemented");
            }
            glShaderSource(_id, 1, &c_s, NULL);
            glCompileShader(_id);
        } catch (std::runtime_error& e) {
            std::string dump(c_s);
            error_logger(e.what());
            error_logger(printLineNos(dump));
            throw e;
        }
        // check for shader compile errors
        int success;
        char infoLog[512];
        glGetShaderiv(_id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(_id, 512, NULL, infoLog);
            mork::error_logger("SHADER COMPILATION FAILED: ", infoLog); 
            mork::error_logger(s);
            throw std::runtime_error(infoLog);
        }
    }



    std::string   Shader::preProcess(const std::string& s) {
        std::stringstream ss(s);

        std::string line;
        std::string processed;
        int i = 0;


        while(std::getline(ss, line)) {
            // Line no;
            ++i;        
            
            if(line.find("#include") != std::string::npos) {
                std::string include = line.substr(8, line.length());

                // Remove hyphens and whitespace
                include.erase(std::remove(include.begin(), include.end(), ' '), include.end());
                include.erase(std::remove(include.begin(), include.end(), '\"'), include.end());
                mork::debug_logger("Ecountered include statement, including file: [", include, "]");
                std::ifstream ifs(include);
                if(!ifs.is_open()) {
                    mork::error_logger("Could not open file \"", include, "\".");
                    throw std::runtime_error("Error preprocessing shader");

                }
                std::string include_str;
                std::string iline;
                int j = 0;
                while(std::getline(ifs, iline)) {
                   ++j;
                   include_str.append(iline); 
                   include_str.append("\n");
                }
                mork::debug_logger("Read ", j, " lines: ");
                //mork::debug_logger(include_str);

                // trim last endline, since it will be reappended when adde to outer
                if(include_str[include_str.length()-1] == '\n')
                    include_str = include_str.substr(0, include_str.length()-1);

                // Recursively preprocess the included shader
                preProcess(include_str);

                // replace the include statement with the actual included file:
                line = include_str;
            }

            processed.append(line);
            processed.append("\n");
        }

        return processed;
    }

    Program::Program() 
        : _programID(0)
    {

    }


Program::Program(int version, const std::string& src_path)
    : _programID(0)
{
    std::ifstream t(src_path);
    if(t.fail()) {
        error_logger("File ", src_path, " not found");
        throw std::runtime_error("File not found");

    }
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string src = buffer.str();

    if(src.find("_VERTEX_") == std::string::npos || src.find("_FRAGMENT_") == std::string::npos) {
        std::string error = "Invalid shader program, must define at least a _VERTEX_ and a _FRAGMENT_ part";
        error_logger(error);
        
        throw std::runtime_error(error);

    }


    Shader vs(version, src, Shader::Type::VERTEX, "_VERTEX_");
    Shader fs(version, src, Shader::Type::FRAGMENT, "_FRAGMENT_");

    if(src.find("_GEOMETRY_") != std::string::npos) {
        Shader gs(version, src, Shader::Type::GEOMETRY, "_GEOMETRY_");
        buildProgram({vs, gs, fs});
    } else
        buildProgram({vs, fs});

    

}

Program::Program(const std::string& vssrc, const std::string& fssrc)
 : Program(vssrc, "", fssrc)
{

}

Program::Program(const std::string& vssrc, const std::string& gssrc, const std::string& fssrc)
 : _programID(0)
{
    std::vector<std::reference_wrapper<Shader> > shaders;

    Shader vs(330, vssrc, Shader::Type::VERTEX);
    Shader fs(330, fssrc, Shader::Type::FRAGMENT);
    if(!gssrc.empty()) {
        Shader gs(330, gssrc, Shader::Type::GEOMETRY);
        buildProgram({vs, gs, fs});
    } else
        buildProgram({vs, fs});

}


Program::~Program() 
{
    // TODO: Change method of establishing active context
    // (do not use GLFWWindow)
    if(GlfwWindow::isContextActive())
    {
        if(_programID)
            glDeleteProgram(_programID);
    }   
}

Program::Program(Program&& o) noexcept {
    _programID = o._programID;
    o._programID = 0;

    uniforms = std::move(o.uniforms);

}

Program& Program::operator=(Program&& o) noexcept {
    if(GlfwWindow::isContextActive())
    {
        if(_programID)
            glDeleteProgram(_programID);
    }   
    
    _programID = o._programID;
    o._programID = 0;  

    uniforms = std::move(o.uniforms);

    return *this;
}


void    Program::buildProgram(const std::vector<std::reference_wrapper<Shader> >& shaders) {
    // This will delete the program if it allready exist, and detach shaders
    // (Those allready marked for deletion and will be freed)
    if(_programID!=0) {
        glDeleteProgram(_programID);
        _programID = 0;
    }
     
    // Create program and attach shaders
    _programID = glCreateProgram();
    for(auto& sh : shaders)
        glAttachShader(_programID, sh.get().getId());

    glLinkProgram(_programID);
    // check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(_programID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(_programID, 512, NULL, infoLog);
        mork::error_logger("SHADER::PROGRAM::LINKING_FAILED: ", infoLog);
        throw std::runtime_error(infoLog);
    }

    // Establish active non-block uniforms in the program
    uniforms.clear();

    int numUniforms = 0;
    glGetProgramInterfaceiv(_programID, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);
    const GLenum properties[4] = {GL_BLOCK_INDEX, GL_TYPE, GL_NAME_LENGTH, GL_LOCATION};
	
	mork::info_logger("Querying Uniforms in program ", _programID);
    if(numUniforms==0)
        mork::info_logger(" ** No uniforms in this program");
	for(int unif = 0; unif < numUniforms; ++unif)
	{
	    GLint values[4];
	    glGetProgramResourceiv(_programID, GL_UNIFORM, unif, 4, properties, 4, NULL, values);

	    // Skip any uniforms that are in a block.
	    if(values[0] != -1)
		    continue;

	    std::vector<char> nameData(values[2]);
	    glGetProgramResourceName(_programID, GL_UNIFORM, unif, nameData.size(), NULL, &nameData[0]);
	    std::string name(nameData.begin(), nameData.end() - 1);
		int type = values[1];
        int location = values[3];
        mork::info_logger("Name: \"", name, "\", type: ", type, ", location: ", location);
        uniforms.insert({name, Uniform(type, location)});
    }
}

void    Program::use() const
{
    if(!_programID)
        throw std::runtime_error("Program was not created before attempting to use it");
    glUseProgram(_programID);
 
}

int Program::getProgramId() const
{
    return _programID;
}

const Uniform& Program::getUniform(const std::string& name) const {
    auto entry = uniforms.find(name);
    if(entry==uniforms.end()) {
        mork::error_logger("Tried to acess uniform \"", name, "\" which is not active in program ", _programID);
        throw std::runtime_error("Error acessing Uniform, see log");
    } 
    
    return entry->second;
}
 
bool Program::queryUniform(const std::string& name) const {
    auto entry = uniforms.find(name);
    if(entry==uniforms.end()) {
        return false;
    } 
    
    return true;
}

bool Program::bindTexture(const TextureBase& tex, const std::string& name, int texUnit) const {
    const Uniform& u = this->getUniform(name);
    auto type = u.getType();
    if(type!=GL_SAMPLER_3D && type!=GL_SAMPLER_2D && type != GL_SAMPLER_CUBE) {
        error_logger("Program::BindTexture() can only be used on uniforms of texture type. Requested uniform was \"", name, "\" of type ", type);
        throw std::runtime_error(error_logger.last());
    }   

    u.set(texUnit);
    tex.bind(texUnit);
}



inline json programSchema = R"(
    {
        "$schema": "http://json-schema.org/draft-07/schema#",
        "title": "Program schema",
        "type": "object",
        "description": "A program object",
        "properties": {
            "name": { "type": "string" },
            "source": { "type": "string" },
            "version": { "type": "integer" } 
        },
        "required": ["source"],
        "additionalProperties": false
    }
    )"_json;

    class ProgramResource: public ResourceTemplate<Program>
    {
		public:
		    ProgramResource(ResourceManager& manager, Resource& r) :
				ResourceTemplate<Program>(programSchema)
			{
	            info_logger("Resource - Program");
         	    const json& js = r.getDescriptor();
                validator.validate(js);

                path = js["source"].get<std::string>();

                version = 330;
                if(js.count("version"))
                    version = js["version"].get<int>();

            }

            Program releaseResource() {
				return Program(version, path);
            }
		private:
            std::string path;
            int version;

    };

    inline std::string program = "program";

    static ResourceFactory<Program>::Type<program, ProgramResource> ProgramType;


inline json programPoolSchema = R"(
    {
        "$schema": "http://json-schema.org/draft-07/schema#",
        "title": "ProgramPool schema",
        "type": "object",
        "description": "A program pool object",
        "properties": {
            "name": { "type": "string" },
            "programs": { 
                "type": "array",
                "items": {"type": "object" }
            }
        },
        "additionalProperties": false
    }
    )"_json;

    class ProgramPoolResource: public ResourceTemplate<ProgramPool>
    {
		public:
		    ProgramPoolResource(ResourceManager& manager, Resource& r) :
				ResourceTemplate<ProgramPool>(programPoolSchema)
			{
	            info_logger("Resource - ProgramPool");
         	    const json& js = r.getDescriptor();
                validator.validate(js);

                json parray = js["programs"];
                for(auto& arrayObject : parray) {
                    auto& name = arrayObject["name"];
                    Resource& prog_r = r.addChildResource(Resource(manager, "program", arrayObject, arrayObject["source"]));
                    auto prog = ResourceFactory<Program>::getInstance().create(manager, prog_r);
                    
                    pp.insert({name,std::move(prog)});    
                }                

            }

            ProgramPool releaseResource() {
				return std::move(pp);
            }
		private:
            ProgramPool pp;			

    };

    inline std::string programPool = "programPool";

    static ResourceFactory<ProgramPool>::Type<programPool, ProgramPoolResource> ProgramPoolType;

}
