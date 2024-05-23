#pragma once

#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>

//custom
#include "obj_loader.h"
//#include "shader.h"
#include "render_data.h"
#include "renderpass.h"
#include "renderPipeline.h"
#include "utilities.h"

//imgui
#include "imgui.h"
#include "backend/imgui_impl_glfw.h"
#include <backend/imgui_impl_opengl3.h>