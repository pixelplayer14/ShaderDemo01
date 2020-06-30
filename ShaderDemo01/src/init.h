extern const int screenSizeX;
extern const int screenSizeY;
void WindowResizeCallback(GLFWwindow* window, int width, int height);
void GlClearError();
void GlCheckError();
std::string readShader();
int CompileShader(const std::string& source, unsigned int type);
int CreateShader(const std::string source);
int setupSquare();