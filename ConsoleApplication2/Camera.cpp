#include "Camera.hpp"

Camera::Camera(GLFWwindow* window)
{
	programs = std::vector<GLuint>();
	matrixIDs = std::vector<GLuint>();
	viewPosIDs = std::vector<GLuint>();
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	position = glm::vec3(10, 10, 10);

	// horizontal angle : toward -Z
	horizontalAngle = 0.0f;
	// vertical angle : 0, look at the horizon
	verticalAngle = 0.0f;
	// Initial Field of View
	initialFoV = 45.0f;
	lastTime = glfwGetTime();
	glfwSetScrollCallback(window, scroll_callback);
	speed = 5.0f; // 3 units / second
	mouseSpeed = 0.20f;
	this->window = window;
	
	glm::mat4 Projection = glm::perspective(glm::radians(initialFoV), 4.0f / 3.0f, 0.1f, 1000.0f);

	// Camera matrix
	glm::mat4 View = glm::lookAt(
		position, 
		glm::vec3(0, 0, 0), // origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	// Our ModelViewProjection : multiplication of our 3 matrices
	MVP = Projection * View; // Remember, matrix multiplication is the other way around
}

Camera::~Camera()
{
}
void Camera::update()
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);

    float currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);
    lastTime = currentTime;
    horizontalAngle += mouseSpeed * deltaTime * float(WIDTH / 2 - xpos);
    verticalAngle += mouseSpeed * deltaTime * float(HEIGHT / 2 - ypos);

    // Clamp the vertical angle to prevent the camera from flipping upside down
    const float maxVerticalAngle = glm::radians(89.0f);
    if (verticalAngle > maxVerticalAngle)
        verticalAngle = maxVerticalAngle;
    else if (verticalAngle < -maxVerticalAngle)
        verticalAngle = -maxVerticalAngle;

    glm::vec3 direction(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
    );

    glm::vec3 right = glm::vec3(
        sin(horizontalAngle - 3.14f / 2.0f),
        0,
        cos(horizontalAngle - 3.14f / 2.0f)
    );
    glm::vec3 up = glm::cross(right, direction);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_REPEAT) {
        position += direction * deltaTime * speed;
    }
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_REPEAT) {
        position -= direction * deltaTime * speed;
    }
    // Strafe right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_REPEAT) {
        position += right * deltaTime * speed;
    }
    // Strafe left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_REPEAT) {
        position -= right * deltaTime * speed;
    }
    // Move up
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_REPEAT) {
        position += up * deltaTime * speed;
    }
    // Move down
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_REPEAT) {
        position -= up * deltaTime * speed;
    }

    glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(initialFoV), 4.0f / 3.0f, 0.1f, 100.0f);
    // Camera matrix
    glm::mat4 ViewMatrix = glm::lookAt(
        position,           // Camera is here
        position + direction, // and looks here : at the same position, plus "direction"
        up                  // Head is up (set to 0,-1,0 to look upside-down)
    );

    setMVP(ProjectionMatrix * ViewMatrix);
    glUseProgram(textProgramID);
    glUniformMatrix4fv(textMatrixID, 1, GL_FALSE, &ProjectionMatrix[0][0]);

    for (int i = 0; i < programs.size(); i++) {
        glUseProgram(programs[i]);
        glUniformMatrix4fv(matrixIDs[i], 1, GL_FALSE, &MVP[0][0]);
        glUniform3fv(viewPosIDs[i], 1, &position[0]);
    }
}


void Camera::addProgram(GLuint programID)
{
	programs.push_back(programID);
	matrixIDs.push_back(glGetUniformLocation(programID, "VP"));
	viewPosIDs.push_back(glGetUniformLocation(programID, "viewPos"));
}

void Camera::addTextShader(GLuint programID)
{
	textProgramID = programID;
	textMatrixID = glGetUniformLocation(programID, "P");
}


