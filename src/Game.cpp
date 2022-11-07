#include "../header/Game.h"

Game::Game(bool logging) {
	ScreenResolution = {0, 0};
	prevScreenResolution = {0, 0};
	log = logging;
}

Game::Game(glm::vec2 screen_resolution, bool logging) {
	ScreenResolution = screen_resolution;
	prevScreenResolution = screen_resolution;
	log = logging;
}

void Game::Run() {
	Setup();
}

void Game::Setup() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWvidmode mode = *glfwGetVideoMode(glfwGetPrimaryMonitor());
	if (ScreenResolution.x <= 0 || ScreenResolution.y <= 0) {
		ScreenResolution = glm::vec2{ mode.width, mode.height } *0.8f;
		prevScreenResolution = ScreenResolution;
	}

	GLFWmonitor* monitor = NULL;
	window = 
		glfwCreateWindow(
			(int)ScreenResolution.x, 
			(int)ScreenResolution.y, 
			"Monorush", 
			monitor, 
			NULL
		);
	if (!window) {
		std::cout << "ERROR Window " << window << "\n";
		glfwTerminate();
		return;
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();

	WindowPos = glm::vec2{ (int)(mode.width - ScreenResolution.x), (int)(mode.height - ScreenResolution.y) } / 2.0f;
	glfwSetWindowPos(
		window, 
		(int)WindowPos.x, 
		(int)WindowPos.y
	);

	glfwSetWindowSizeCallback(window, Game::WindowResizeCallback);
	glfwSetScrollCallback(window, Input::ScanMouseScroll);
	Input::SetWindowInput(window);

	glfwSetWindowSizeLimits(window, 640, 360, GLFW_DONT_CARE, GLFW_DONT_CARE);
	glfwSetWindowAspectRatio(window, 16, 9);
	//glfwSwapInterval(0);

#ifdef MAGIA_DEBUG
	SetIcon("../texture/icon.png");
#else
	SetIcon("texture/icon.png");
#endif

	glViewport(0, 0, (int)ScreenResolution.x, (int)ScreenResolution.y);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Audio::log = true;
	UI::log = false;
	Texture::log = false;
	Shader::log = true;
	Noise::log = true;

	Shader::Init();
	Audio::Init();
	UI::Init();
	Noise::Init();
	Texture::Init();
	Loop();
}

void Game::Loop() {
	glClearColor(0.95f, 0.97f, 1, 1);
	if (!LoadLayer(layerIndex)) return;

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		time._UpdateTime((float)glfwGetTime());

		Input::ScanKey(window);
		Input::ScanMouse(window);
		UI::PollsEvent(window, time);

		if (ProcessLayerState(layerIndex)) {
			UpdateLayer(layerIndex, time);
		}
		else glfwSetWindowShouldClose(window, true);

		Input::ClearInputBuffer();
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	if (log) std::cout << "---------------- CLEAN UP -----------------\n";
	glfwDestroyWindow(window);
	glfwTerminate();
	Audio::Destroy();
	UI::Destroy();
}

void Game::WindowResizeCallback(GLFWwindow* window, int x, int y) {
	glViewport(0, 0, x, y);
}

void Game::SetIcon(std::string file) {
	if (log) std::cout << "LOAD Icon " << file << "\n";
	GLFWimage image{};
	image.pixels = stbi_load(file.c_str(), &image.width, &image.height, 0, 4);
	assert(image.pixels);
	glfwSetWindowIcon(window, 1, &image);
	stbi_image_free(image.pixels);
}

void Game::SetFullscreen(bool status) {
	fullscreen = status;
	if (fullscreen) {
		glm::ivec2 winPos = {0, 0};
		glfwGetWindowPos(window, &winPos.x, &winPos.y);
		WindowPos = winPos;

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		auto mode = glfwGetVideoMode(monitor);
		glm::ivec2 sz = { mode->width, mode->height };
		glfwSetWindowMonitor(window, monitor, 0, 0, sz.x, sz.y, GLFW_DONT_CARE);

		prevScreenResolution = ScreenResolution;
		ScreenResolution = sz;
		glViewport(0, 0, sz.x, sz.y);
	}
	else {
		ScreenResolution = prevScreenResolution;
		glfwSetWindowMonitor(window, NULL, WindowPos.x, WindowPos.y,
			ScreenResolution.x, ScreenResolution.y, GLFW_DONT_CARE);
		glViewport(0, 0, ScreenResolution.x, ScreenResolution.y);
	}
}

void Game::SetVolume(float volume) {
	gameVolumeGain = volume;
	if (gameVolumeGain < 0) gameVolumeGain = 10;
	else if (gameVolumeGain > 10) gameVolumeGain = 0;

	Audio::SetListenerGain(gameVolumeGain);
}

void Game::ClearLayer(int layerIndex) {
	switch (layerIndex) {
		case 0:
			menuLayer.reset();
			break;
		case 1:
			gameLayer.reset();
			break;
		default:
			break;
	}
}

bool Game::LoadLayer(int layerIndex) {
	switch (layerIndex) {
	case 0:
		this->layerIndex = 0;
		menuLayer = std::make_shared<MenuLayer>();
		UpdatePacket(*menuLayer->state);
		ProcessLayerState(0);
		menuLayer->OnAttach();
		return true;
	case 1:
		this->layerIndex = 1;
		gameLayer = std::make_shared<GameLayer>();
		UpdatePacket(*gameLayer->state);
		ProcessLayerState(1);
		gameLayer->OnAttach();
		return true;
	default:
		return false;
	}
}

void Game::UpdateLayer(int layerIndex, Time time) {
	switch (layerIndex) {
	case 0:
		menuLayer->OnUpdate(time);
		break;
	case 1:
		gameLayer->OnUpdate(time);
		break;
	default:
		break;
	}
}

void Game::UpdatePacket(LayerState& state) {
	glm::ivec2 winSize = { 1280, 720 };
	glfwGetWindowSize(window, &winSize.x, &winSize.y);

	if (!fullscreen && ScreenResolution != winSize)
		prevScreenResolution = winSize;
	ScreenResolution = winSize;

	state.window = window;
	state.volumeGain = gameVolumeGain;
	state.currentSceneIndex = layerIndex;
	state.fullScreen = fullscreen;
	state.resolution = ScreenResolution;
}

bool Game::ProcessLayerState(int layerIndex) {
	Ref<LayerState> layerState = NULL;

	switch (layerIndex) {
	case 0:
		layerState = menuLayer->state;
		break;
	case 1:
		layerState = gameLayer->state;
		break;
	default:
		return false;
	}

	if (!layerState->update) {
		UpdatePacket(*layerState);
		return true;
	}

	if (layerState->terminate) glfwSetWindowShouldClose(window, true);

	if (layerState->fullScreen != this->fullscreen) {
		SetFullscreen(layerState->fullScreen);
	}

	if (layerState->volumeGain != this->gameVolumeGain) {
		SetVolume(layerState->volumeGain);
	}

	if (layerState->reload) {
		ClearLayer(layerIndex);
		if (!LoadLayer(layerIndex)) return false;
		if (log) std::cout << "RELOAD Scene " << layerIndex << "\n";
		return true;
	}

	if (layerState->currentSceneIndex != this->layerIndex) {
		int prevLayer = this->layerIndex;
		this->layerIndex = glm::clamp<int>(layerState->currentSceneIndex, 0, 256);
		if (log) std::cout << "SWITCH Scene from " << prevLayer << " to " << this->layerIndex << "\n";
		ClearLayer(prevLayer);
		if (!LoadLayer(this->layerIndex)) return false;
	}

	layerState.reset();
	layerState = std::make_shared<LayerState>();
	UpdatePacket(*layerState);
	return true;
}