#include <shader-ai.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace {
std::string readTextFile(const std::string& path) {
    std::ifstream f(path, std::ios::in | std::ios::binary);
    if (!f) throw std::runtime_error("Shader: cannot open '" + path + "'");
    std::ostringstream s; s << f.rdbuf(); return s.str();
}
GLuint compileStage(GLenum type, const std::string& source, const std::string& label) {
    GLuint sh = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(sh, 1, &src, nullptr);
    glCompileShader(sh);
    GLint ok = GL_FALSE; glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len = 0; glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(std::max(len, 1));
        glGetShaderInfoLog(sh, len, nullptr, log.data());
        glDeleteShader(sh);
        throw std::runtime_error("Shader [" + label + "] compile failed:\n" + log.data());
    }
    return sh;
}
}

Shader::Shader(const std::string& vs, const std::string& fs) { loadFromFiles(vs, fs); }
Shader::~Shader() { destroy(); }
Shader::Shader(Shader&& o) noexcept : m_program(o.m_program) { o.m_program = 0; }
Shader& Shader::operator=(Shader&& o) noexcept {
    if (this != &o) { destroy(); m_program = o.m_program; o.m_program = 0; }
    return *this;
}
void Shader::destroy() { if (m_program) { glDeleteProgram(m_program); m_program = 0; } }

void Shader::loadFromFiles(const std::string& vs, const std::string& fs) {
    loadFromSource(readTextFile(vs), readTextFile(fs));
}
void Shader::loadFromSource(const std::string& vs, const std::string& fs) {
    GLuint v = compileStage(GL_VERTEX_SHADER, vs, "vertex");
    GLuint f = compileStage(GL_FRAGMENT_SHADER, fs, "fragment");
    GLuint p = glCreateProgram();
    glAttachShader(p, v); glAttachShader(p, f); glLinkProgram(p);
    glDeleteShader(v);    glDeleteShader(f);
    GLint ok = GL_FALSE; glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len = 0; glGetProgramiv(p, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(std::max(len, 1));
        glGetProgramInfoLog(p, len, nullptr, log.data());
        glDeleteProgram(p);
        throw std::runtime_error(std::string("Shader link failed:\n") + log.data());
    }
    destroy(); m_program = p;
}
void Shader::bind()   const { glUseProgram(m_program); }
void Shader::unbind()       { glUseProgram(0); }

GLint Shader::uniformLocation(const std::string& n) const {
    return glGetUniformLocation(m_program, n.c_str());
}
void Shader::setFloat(const std::string& n, float v) const { glUniform1f(uniformLocation(n), v); }
void Shader::setInt  (const std::string& n, int v)   const { glUniform1i(uniformLocation(n), v); }
void Shader::setVec3 (const std::string& n, const glm::vec3& v) const { glUniform3fv(uniformLocation(n), 1, &v[0]); }
void Shader::setMat4 (const std::string& n, const glm::mat4& m) const { glUniformMatrix4fv(uniformLocation(n), 1, GL_FALSE, &m[0][0]); }