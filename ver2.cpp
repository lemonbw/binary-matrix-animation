#include <SDL.h>
#include <SDL_ttf.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <string>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int FONT_SIZE = SCREEN_HEIGHT / 30;

class Symbol {
public:
    Symbol(int x, int y, int speed, TTF_Font* font)
        : x(x), y(y), speed(speed), font(font), alpha(0), isTransparent(true) {
        value = renderRandomSymbol();
        changeInterval = 300;
        lastChangeTime = SDL_GetTicks();
    }

    ~Symbol() {
        if (value) {
            SDL_FreeSurface(value);
        }
    }

    void update() {
        Uint32 currentTime = SDL_GetTicks();

        if (currentTime - lastChangeTime >= changeInterval) {
            if (value) {
                SDL_FreeSurface(value);
            }
            value = renderRandomSymbol();
            lastChangeTime = currentTime;
        }

        if (isTransparent) {
            if (alpha < rand() % (255 - 100) + 100) {
                alpha += 5;
            } else {
                isTransparent = false;
            }
        } else {
            if (alpha > rand() % 10) {
                alpha -= 5;
            } else {
                isTransparent = true;
            }
        }

        y += speed;
        if (y > SCREEN_HEIGHT) {
            y = -FONT_SIZE;
        }
    }

    void draw(SDL_Renderer* renderer) {
        if (!value) return;

        SDL_Texture* tempTexture = SDL_CreateTextureFromSurface(renderer, value);
        if (!tempTexture) {
            SDL_Log("Failed to create texture: %s", SDL_GetError());
            return;
        }

        SDL_SetTextureAlphaMod(tempTexture, alpha);
        SDL_Rect dstRect = { x, y, FONT_SIZE, FONT_SIZE };
        SDL_RenderCopy(renderer, tempTexture, nullptr, &dstRect);
        SDL_DestroyTexture(tempTexture);
    }

    void setIsTransparent(bool transparent) {
        isTransparent = transparent; 
    }

private:
    int x, y, speed, alpha;
    Uint32 changeInterval, lastChangeTime;
    TTF_Font* font;
    SDL_Surface* value;

    SDL_Surface* renderRandomSymbol() {
        int randomValue = rand() % 2;
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, std::to_string(randomValue).c_str(), { 0, 255, 0 });
        if (!textSurface) {
            SDL_Log("Failed to render text: %s", TTF_GetError());
        }
        return textSurface;
    }

    bool isTransparent; 
};

class SymbolColumn {
public:
    SymbolColumn(int x, int y, TTF_Font* font) {
        columnHeight = rand() % (25 - 8) + 8;
        speed = rand() % (6 - 3) + 3;
        for (int i = 0; i < columnHeight; ++i) {
            symbols.emplace_back(x, y - FONT_SIZE * i, speed, font);
        }

        for (size_t i = 0; i < symbols.size() / 2; ++i) {
            symbols[i].setIsTransparent(true);
        }
    }

    void update() {
        if (rand() % 2) {
            int randIndex = rand() % (symbols.size() / 2);
            symbols[randIndex].setIsTransparent(false);
        }

        for (auto& symbol : symbols) {
            symbol.update();
        }
    }

    void draw(SDL_Renderer* renderer) {
        for (auto& symbol : symbols) {
            symbol.draw(renderer);
        }
    }

private:
    int columnHeight, speed;
    std::vector<Symbol> symbols;
};

#ifdef _WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    return main(__argc, __argv);
}
#endif

int main(int argc, char* argv[]) {
    srand(static_cast<unsigned int>(time(0)));

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    if (TTF_Init() == -1) {
        SDL_Log("Failed to initialize TTF: %s", TTF_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Matrix Rain", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (!window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("fonts/Menlo-Regular.ttf", FONT_SIZE);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    std::vector<SymbolColumn> symbolColumns;
    for (int x = 0; x < SCREEN_WIDTH; x += FONT_SIZE) {
        symbolColumns.emplace_back(x, rand() % SCREEN_HEIGHT, font);
    }

    bool running = true;
    Uint32 lastFrameTime = SDL_GetTicks();

    while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
    }

    Uint32 currentFrameTime = SDL_GetTicks();
    Uint32 deltaTime = currentFrameTime - lastFrameTime;
    lastFrameTime = currentFrameTime;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    for (auto& column : symbolColumns) {
        column.update();
        column.draw(renderer);
    }

    SDL_RenderPresent(renderer);

    SDL_Delay(16);
    }


    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
