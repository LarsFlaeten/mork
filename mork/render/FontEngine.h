#ifndef _MORK_FONTENGINE_H_
#define _MORK_FONTENGINE_H_

#include <map>
#include <memory>
#include "mork/render/Font.h"
#include "mork/render/Program.h"

namespace mork {

    class FontEngine {
        public:
        static void init();
        static Program& getProgram();

        static Font& getFont(const std::string& name);

        private:        
        static std::map<std::string, std::shared_ptr<Font> > fonts;
       
        static std::string font_vs;
        static std::string font_fs;
        
        static std::shared_ptr<Program>  fontProgram;

    };



}


#endif
