int SCREEN_WIDTH = 664;
int SCREEN_HEIGHT = 664;

//Texture wrapper class
class LTexture {
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

	//Loads image at specified path
	bool loadFromFile(std::string path);

#ifdef _SDL_TTF_H
	//Creates image from font string
	bool loadFromRenderedText(std::string textureText, SDL_Color textColor,
			TTF_Font *gFont);
#endif

	//Deallocates texture
	void free();

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	//Set blending
	void setBlendMode(SDL_BlendMode blending);

	//Set alpha modulation
	void setAlpha(Uint8 alpha);

	//Renders texture at given point
	void render(int x, int y, int w, int h, SDL_Rect* clip = NULL,
			double angle = 0.0, SDL_Point* center = NULL,
			SDL_RendererFlip flip = SDL_FLIP_NONE);

	//Gets image dimensions
	int getWidth();
	int getHeight();

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};

class LWindow {
public:
	//Intializes internals
	LWindow();

	//Creates window
	bool init();

	//Creates renderer from internal window
	SDL_Renderer* createRenderer();

	//Handles window events
	void handleEvent(SDL_Event& e);

	//Deallocates internals
	void free();

	//Window dimensions
	int getWidth();
	int getHeight();

	//Window focii
	bool hasMouseFocus();
	bool hasKeyboardFocus();
	bool isMinimized();

private:
	//Window data
	SDL_Window* mWindow;

	//Window dimensions
	int mWidth;
	int mHeight;

	//Window focus
	bool mMouseFocus;
	bool mKeyboardFocus;
	bool mFullScreen;
	bool mMinimized;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Our custom window
LWindow gWindow;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Globally used font
TTF_Font *gFont25 = NULL;
TTF_Font *gFontMunro = NULL;
TTF_Font *gFontMunro16 = NULL;
TTF_Font *gFontMunro19 = NULL;

//LTexture gTextImage;
LTexture gText;
LTexture gBlock;


LTexture::LTexture() {
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture() {
	//Deallocate
	free();
}

bool LTexture::loadFromFile(std::string path) {
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL) {
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(),
		IMG_GetError());
	} else {
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE,
				SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL) {
			printf("Unable to create texture from %s! SDL Error: %s\n",
					path.c_str(), SDL_GetError());
		} else {
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText(std::string textureText,
		SDL_Color textColor, TTF_Font *gFont) {
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(),
			textColor);
	if (textSurface != NULL) {
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL) {
			printf(
					"Unable to create texture from rendered text! SDL Error: %s\n",
					SDL_GetError());
		} else {
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	} else {
		printf("Unable to render text surface! SDL_ttf Error: %s\n",
		TTF_GetError());
	}

	//Return success
	return mTexture != NULL;
}
#endif

void LTexture::free() {
	//Free texture if it exists
	if (mTexture != NULL) {
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending) {
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha) {
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, int w, int h, SDL_Rect* clip, double angle,
		SDL_Point* center, SDL_RendererFlip flip) {
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, w, h };

	//Set clip rendering dimensions
	if (clip != NULL) {
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center,
			flip);
}

int LTexture::getWidth() {
	return mWidth;
}

int LTexture::getHeight() {
	return mHeight;
}

LWindow::LWindow() {
	//Initialize non-existant window
	mWindow = NULL;
	mMouseFocus = false;
	mKeyboardFocus = false;
	mFullScreen = false;
	mMinimized = false;
	mWidth = 0;
	mHeight = 0;
}

bool LWindow::init() {
	//Create window
	mWindow = SDL_CreateWindow("Game of Life by Carl Mesias",
			SDL_WINDOWPOS_CENTERED,
	SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT,
			SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);// | SDL_WINDOW_MAXIMIZED);
	if (mWindow != NULL) {
		mMouseFocus = true;
		mKeyboardFocus = true;
		mWidth = SCREEN_WIDTH;
		mHeight = SCREEN_HEIGHT;
	}

	return mWindow != NULL;
}

SDL_Renderer* LWindow::createRenderer() {
	return SDL_CreateRenderer(mWindow, -1,
			SDL_RENDERER_ACCELERATED );//| SDL_RENDERER_PRESENTVSYNC);
}

void LWindow::handleEvent(SDL_Event& e) {
	//Window event occured
	if (e.type == SDL_WINDOWEVENT) {
		//Caption update flag
		bool updateCaption = false;

		switch (e.window.event) {
		//Get new dimensions and repaint on window size change
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			mWidth = e.window.data1;
			mHeight = e.window.data2;
			SDL_RenderPresent(gRenderer);
			break;

			//Repaint on exposure
		case SDL_WINDOWEVENT_EXPOSED:
			SDL_RenderPresent(gRenderer);
			break;

			//Mouse entered window
		case SDL_WINDOWEVENT_ENTER:
			mMouseFocus = true;
			updateCaption = true;
			break;

			//Mouse left window
		case SDL_WINDOWEVENT_LEAVE:
			mMouseFocus = false;
			updateCaption = true;
			break;

			//Window has keyboard focus
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			mKeyboardFocus = true;
			updateCaption = true;
			break;

			//Window lost keyboard focus
		case SDL_WINDOWEVENT_FOCUS_LOST:
			mKeyboardFocus = false;
			updateCaption = true;
			break;

			//Window minimized
		case SDL_WINDOWEVENT_MINIMIZED:
			mMinimized = true;
			break;

			//Window maxized
		case SDL_WINDOWEVENT_MAXIMIZED:
			mMinimized = false;
			break;

			//Window restored
		case SDL_WINDOWEVENT_RESTORED:
			mMinimized = false;
			break;
		}

		//Update window caption with new data
		if (updateCaption) {
			std::stringstream caption;
			caption << "Game of Life Clone by Cmesias - MouseFocus:"
					<< ((mMouseFocus) ? "On" : "Off") << " KeyboardFocus:"
					<< ((mKeyboardFocus) ? "On" : "Off");
			SDL_SetWindowTitle(mWindow, caption.str().c_str());
		}
	}
	//Enter exit full screen on return key
	//else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
	else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F11) {
		if (mFullScreen) {
			SDL_SetWindowFullscreen(mWindow, SDL_FALSE);
			mFullScreen = false;
		} else {
			SDL_SetWindowFullscreen(mWindow, SDL_TRUE);
			mFullScreen = true;
			mMinimized = false;
		}
	}
}

void LWindow::free() {
	if (mWindow != NULL) {
		SDL_DestroyWindow(mWindow);
	}

	mMouseFocus = false;
	mKeyboardFocus = false;
	mWidth = 0;
	mHeight = 0;
}

int LWindow::getWidth() {
	return mWidth;
}

int LWindow::getHeight() {
	return mHeight;
}

bool LWindow::hasMouseFocus() {
	return mMouseFocus;
}

bool LWindow::hasKeyboardFocus() {
	return mKeyboardFocus;
}

bool LWindow::isMinimized() {
	return mMinimized;
}

bool init() {
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init( SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	} else {
		//Set texture filtering to linear
		SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1");

		//Create window
		gWindow.init();

		//Create vsynced renderer for window
		gRenderer = gWindow.createRenderer();

		//Initialize renderer color
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

		//Initialize PNG loading
		int imgFlags = IMG_INIT_PNG;
		IMG_Init(imgFlags);
		TTF_Init();

		//SDL_Init(SDL_INIT_EVERYTHING);

	}

	return success;
}

bool loadMedia() {
	bool success = true;

	//Fonts
	gFont25 = TTF_OpenFont("fonts/lsansuni.ttf", 25);
	gFontMunro = TTF_OpenFont("fonts/Munro.ttf", 30);
	gFontMunro16 = TTF_OpenFont("fonts/Munro.ttf", 12);
	gFontMunro19 = TTF_OpenFont("fonts/Munro.ttf", 19);

	gBlock.loadFromFile("block.png");
	gBlock.setAlpha(80);

	return success;
}

void close() {
	gText.free();
	gBlock.free();
	TTF_CloseFont(gFont25);
	TTF_CloseFont(gFontMunro);
	TTF_CloseFont(gFontMunro16);
	TTF_CloseFont(gFontMunro19);
	gFont25 = NULL;
	gFontMunro = NULL;
	gFontMunro16 = NULL;
	gFontMunro19 = NULL;

	//Destroy window
	SDL_DestroyRenderer(gRenderer);
	gWindow.free();

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}
