#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

GLfloat point[] = {
    0.0f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f
};

GLfloat colors[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
};

const char* vertex_shader =
"#version 460\n"
"layout(location = 0) in vec3 vertex_position;"
"layout(location = 1) in vec3 vertex_color;"
"out vec3 color;"
"void main() {"
"   color = vertex_color;"
"   gl_Position = vec4(vertex_position, 1.0);"
"}";

const char* fragment_shader =
"#version 460\n"
"in vec3 color;"
"out vec4 frag_color;"
"void main() {"
"   frag_color = vec4(color, 1.0);"
"}";

int g_windowSizeX = 640;
int g_windowSizeY = 480;

void glfwWindowSizeCallback(GLFWwindow* pWindow, int width, int height)
{
    g_windowSizeX = width;
    g_windowSizeY = height;
    glViewport(0, 0, g_windowSizeX, g_windowSizeY); // Говорим где будем рисовать окно
}

void glfwKeyCallBack(GLFWwindow* pWindow, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(pWindow, GL_TRUE);
    }
}

int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
    {
        std::cout << "glfwInit failed!" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* pWindow = glfwCreateWindow(g_windowSizeX, g_windowSizeY, "Battle City", nullptr, nullptr);
    if (!pWindow)
    {
        std::cout << "glfwCreateWindow failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Регестрируем Callback нашей функции glfwWindowSizeCallback с помошью glfwSetWindowSizeCallback
    // pWindow - для какого окна мы регестрируем наш callback и вторым параметром какую функцыю регестрируем
    glfwSetWindowSizeCallback(pWindow, glfwWindowSizeCallback);
    // Callback функции glfwKeyCallBack для закрытия окна pWindow через esc
    glfwSetKeyCallback(pWindow, glfwKeyCallBack);
    /* Make the window's context current */
    glfwMakeContextCurrent(pWindow);

    if (!gladLoadGL())
    {
        std::cout << "Can't load GLAD!" << std::endl;
        return -1;
    }

    std::cout << "Render: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

    glClearColor(1, 1, 0, 1);

    GLuint vs = glCreateShader(GL_VERTEX_SHADER); // Создаем идентификатор шейдера
    glShaderSource(vs, 1, &vertex_shader, nullptr); // Передаем шейдеру исходный код
    glCompileShader(vs);  // Генерируем шейдер

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER); // Создаем идентификатор шейдера
    glShaderSource(fs, 1, &fragment_shader, nullptr); // Передаем шейдеру исходный код
    glCompileShader(fs);  // Генерируем шейдер

    // Далее будет слинковка шейдеров , то есть превращение их в одну шейдерную программу,
    // а именно связывание выходных данных вертексного шейдера с входными данными фрагментного
    // шейдера.
    GLuint shader_program = glCreateProgram(); // Генерируем шейдерную программу, возвращает 
    // идентифекатор шейдерной программы
    glAttachShader(shader_program, vs);  // Приатачиваем шейдер , shader_program - id шейдерной программы
    // vs - id шейдера. Это вертексного шейдера.
    glAttachShader(shader_program, fs);  // Это фрагментного шейдера.
    glLinkProgram(shader_program);  // Делаем 

    // После линковки вертексный и фрагментный шейдер нам больше не нужен, удаляем.
    glDeleteShader(vs);
    glDeleteShader(fs);

    // Шейдеры готовы и теперь нам нужно передать всю необходимую для них информацию
    // в память видеокарты, а именно позицыю и цвет наших вертексов, для этого
    // мы воспользуемся вертекс баф обджект или VBO.
    GLuint points_vbo = 0; // Создаем идентификатор полученый от драйвера
    glGenBuffers(1, &points_vbo);  // Генерируем glBuffers. Драйвер при вызове этой команды будет 
    // генерировать один вертекс бафер обджект и записываетего значение по идентификатору points_vbo.
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo); // Подключаем сггенерированый буфер и делаем его текущим
    glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW); // Заполняем этот сгенерированый буфер информацыей , позицыями нашего вектора на видеокарте.
    // Эта команда выполняеться для текущего буфера.
    //-------------------------------------------------------
    GLuint colors_vbo = 0;
    glGenBuffers(1, &colors_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    // Указываем GPU что делать с загружеными в нее данными
    GLuint vao = 0; // Генерируем указатель
    glGenVertexArrays(1, &vao);  // Указываем сколько Array нам нужно сгенерировать и делаем указатель на vao
    glBindVertexArray(vao); // Делаем Array текущим

    // Теперь мы свяжем наши points_vbo и colors_vbo с позицыей куда они попадут в шейдер
    glEnableVertexAttribArray(0); // Включаем нулевую позицыю в шейдере
    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);  // Делаем текущим этот буфер (текущим может быть только один буфер !)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);  // Связываем данные.
    // Первый параметр - для какой позиции, второй параметр - сколько мы хотим передать чисел из буфера points_vbo
    // Третий параметр - формат данных, четверый параметр - хотим ли мы нормировать входные данные.
    // Пятый параметр - шаг смещения, Шестой параметр - если наши данные идут со смещением относительно начала массива.

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colors_vbo);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(pWindow))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program); // Используем указаный шийдер 
        glBindVertexArray(vao); // Идентификатор шейдера который будем использовать
        glDrawArrays(GL_TRIANGLES, 0, 3); // Программа для отрисовки текущего(забиндиного) вертекс эрей обджекта (vao)
        /* Swap front and back buffers */
        glfwSwapBuffers(pWindow);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}