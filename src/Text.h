//
// Created by Nemo li on 2021/12/15.
//

#ifndef OGL_TEXT_H
#define OGL_TEXT_H

#include <glad/glad.h>
#include "Shader.h"
#include "map"
#include "Model.h"
#include "ft2build.h"
#include "freetype/ftglyph.h"
#include FT_FREETYPE_H

class Text : Model {
public:
    /// Holds all state information relevant to a character as loaded using FreeType
    struct Character {
        GLuint TextureID;   // ID handle of the glyph texture
        glm::ivec2 Size;    // Size of glyph
        glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
        GLuint Advance;    // Horizontal offset to advance to next glyph
    };

    std::map<GLchar, Character> Characters;
    unsigned int VAO;
    unsigned int VBO;
    Shader *shader;
    static const int MAX_SHORT_VALUE = 65536;

    void
    RenderText(Shader &shader, const wchar_t *text, int textLen, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);

    void init(Shader *shader);

    void loadFacesByUnicode(Shader *shader, const wchar_t *text, int size);
};


#endif //OGL_TEXT_H
