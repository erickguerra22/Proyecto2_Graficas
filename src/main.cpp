#include <SDL.h>
#include <SDL_events.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <SDL_mixer.h>
#include "color.h"
#include "imageloader.h"
#include "raycaster.h"
#include "level.h"

SDL_Window *window;
SDL_Renderer *renderer;

uint32_t frameStart, frameTick = 0;
std::string screenTitle = "Proyecto 2 - Raycasting";
std::string iconPath = "../assets/icon.png";

bool isMenu = true;
bool playMusic = false;
bool win_level = false;
bool win = false;
int selectedLevel = 0;
int option = 0;
bool shooting = false;

Level levels[] = {
    {
        Color(63, 52, 56),
        Color(64, 101, 110),
    },
    {
        Color(86, 52, 53),
        Color(141, 106, 89),
    },
    {
        Color(97, 66, 63),
        Color(217, 172, 129),
    }};

void clear(Color c)
{
  SDL_SetRenderDrawColor(renderer, c.getRed(), c.getGreen(), c.getBlue(), 255);
  SDL_RenderClear(renderer);
}

void draw_floor(Color foorColor)
{
  SDL_SetRenderDrawColor(renderer, foorColor.getRed(), foorColor.getGreen(), foorColor.getBlue(), 255);
  SDL_Rect rect = {
      0,
      SCREEN_HEIGHT / 2,
      SCREEN_WIDTH,
      SCREEN_HEIGHT};
  SDL_RenderFillRect(renderer, &rect);
}

int main(int argv, char **args)
{

  SDL_Init(SDL_INIT_VIDEO);
  Mix_Init(MIX_INIT_MP3);
  ImageLoader::init();

  // Sonidos
  Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
  Mix_Music *menu = Mix_LoadMUS("../assets/menu.mp3");
  Mix_Music *level1 = Mix_LoadMUS("../assets/level_1.mp3");
  Mix_Music *level2 = Mix_LoadMUS("../assets/level_2.mp3");
  Mix_Music *level3 = Mix_LoadMUS("../assets/level_3.mp3");
  Mix_Chunk *fx = Mix_LoadWAV("../assets/shooting.mp3");
  Mix_Chunk *winfx = Mix_LoadWAV("../assets/win.mp3");
  
  SDL_Surface *iconSurface = IMG_Load(iconPath.c_str());

  window = SDL_CreateWindow(screenTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  SDL_SetWindowIcon(window, iconSurface);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  ImageLoader::loadImage("1+", "../assets/wall1_2.png");
  ImageLoader::loadImage("1-", "../assets/wall1_1.png");
  ImageLoader::loadImage("1|", "../assets/wall1_1.png");
  ImageLoader::loadImage("1*", "../assets/wall1_3.png");
  ImageLoader::loadImage("1g", "../assets/goal_1.png");

  // Nivel 2
  ImageLoader::loadImage("2+", "../assets/wall2_2.png");
  ImageLoader::loadImage("2-", "../assets/wall2_1.png");
  ImageLoader::loadImage("2|", "../assets/wall2_1.png");
  ImageLoader::loadImage("2*", "../assets/wall2_3.png");
  ImageLoader::loadImage("2g", "../assets/goal_2.png");

  // Nivel 3
  ImageLoader::loadImage("3+", "../assets/wall3_2.png");
  ImageLoader::loadImage("3-", "../assets/wall3_1.png");
  ImageLoader::loadImage("3|", "../assets/wall3_1.png");
  ImageLoader::loadImage("3*", "../assets/wall3_3.png");
  ImageLoader::loadImage("3g", "../assets/goal_3.png");

  // Pantallas
  ImageLoader::loadImage("menu_1", "../assets/menu_1.png");
  ImageLoader::loadImage("menu_2", "../assets/menu_2.png");
  ImageLoader::loadImage("menu_3", "../assets/menu_3.png");
  ImageLoader::loadImage("win_menu_1", "../assets/level_completed_1.png");
  ImageLoader::loadImage("win_menu_2", "../assets/level_completed_2.png");
  ImageLoader::loadImage("win", "../assets/game_completed.png");
  ImageLoader::loadImage("mapBackground", "../assets/mapBackground.png");

  // Sprites
  ImageLoader::loadImage("player", "../assets/player.png");
  ImageLoader::loadImage("gun", "../assets/blaster.png");
  ImageLoader::loadImage("shoot", "../assets/blasterShooting.png");

  Raycaster r = {renderer};

  clear(levels[selectedLevel].skyColor);

  bool running = true;
  int speed = 10;
  while (running)
  {
    frameStart = SDL_GetTicks();
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
      {
        running = false;
        break;
      }
      if (event.type == SDL_KEYDOWN)
      {
        switch (event.key.keysym.sym)
        {
        case SDLK_a:
          if (!isMenu && !win)
          {
            r.player.a += 3.14 / 24;
            if (r.player.a > 2 * M_PI)
              r.player.a -= 2 * M_PI;
          }
          break;
        case SDLK_d:
          if (!isMenu && !win)
          {
            r.player.a -= 3.14 / 24;
            if (r.player.a < 2 * M_PI)
              r.player.a -= 2 * M_PI;
          }
          break;
        case SDLK_w:
          if (isMenu)
          {
            if (selectedLevel > 0)
            {
              selectedLevel--;
            }
            else
              selectedLevel = 2;
          }
          else if (win_level)
          {
            if (option == 1)
            {
              option = 0;
            }
            else
              option = 1;
          }
          else if (!win && !win_level && !isMenu)
          {
            int nextX = r.player.x + 2 * speed * cos(r.player.a);
            int nextY = r.player.y + 2 * speed * sin(r.player.a);

            if (r.map[nextY / BLOCK][nextX / BLOCK] == ' ')
            {
              r.player.x += speed * cos(r.player.a);
              r.player.y += speed * sin(r.player.a);
            }
            else if (r.map[nextY / BLOCK][nextX / BLOCK] == 'g')
            {
              if (selectedLevel == 2)
                win = true;
              else
                win_level = true;
              Mix_HaltMusic();
              Mix_PlayChannel(0, winfx, 0);
              playMusic = false;
            }
          }
          break;
        case SDLK_s:
          if (isMenu)
          {
            if (selectedLevel < 2)
            {
              selectedLevel++;
            }
            else
              selectedLevel = 0;
          }
          else if (win_level)
          {
            if (option == 0)
            {
              option = 1;
            }
            else
              option = 0;
          }
          else if (!win && !win_level && !isMenu)
          {

            int nextX = r.player.x - speed * cos(r.player.a);
            int nextY = r.player.y - speed * sin(r.player.a);

            if (r.map[nextY / BLOCK][nextX / BLOCK] == ' ')
            {
              r.player.x -= speed * cos(r.player.a);
              r.player.y -= speed * sin(r.player.a);
            }
          }
          break;
        case SDLK_RETURN:
          if (isMenu)
          {
            win_level = false;
            win = false;
            r.load_map("../assets/map" + std::to_string(selectedLevel + 1) + ".txt");
            isMenu = false;
            if (selectedLevel == 0)
              Mix_PlayMusic(level1, -1);
            if (selectedLevel == 1)
              Mix_PlayMusic(level2, -1);
            if (selectedLevel == 2)
              Mix_PlayMusic(level3, -1);
          }
          else if (win_level && option == 0)
          {
            selectedLevel++;
            r.load_map("../assets/map" + std::to_string(selectedLevel) + ".txt");
            r.player.x = BLOCK + 1;
            r.player.y = BLOCK + 1;
            win_level = false;
            if (selectedLevel == 0)
              Mix_PlayMusic(level1, -1);
            if (selectedLevel == 1)
              Mix_PlayMusic(level2, -1);
            if (selectedLevel == 2)
              Mix_PlayMusic(level3, -1);
          }
          else if (win_level && option == 1)
          {
            isMenu = true;
          }
          else if (win)
          {
            selectedLevel = 0;
            isMenu = true;
          }
          break;
        case SDLK_ESCAPE:
          if (!isMenu)
          {
            isMenu = true;
            playMusic = false;
          }
          else if (isMenu)
          {
            running = false;
          }
        default:
          break;
        }
      }
      if (event.type == SDL_MOUSEMOTION)
      {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        if (mouseX >= SCREEN_WIDTH - SCREEN_WIDTH / 3)
        {
          if (!isMenu && !win)
          {
            r.player.a -= 3.14 / 24;
            if (r.player.a > 2 * M_PI)
              r.player.a -= 2 * M_PI;
          }
          break;
        }
        if (mouseX <= SCREEN_WIDTH / 3)
        {
          if (!isMenu && !win)
          {
            r.player.a += 3.14 / 24;
            if (r.player.a < 2 * M_PI)
              r.player.a -= 2 * M_PI;
          }
          break;
        }
      }
      if (event.type == SDL_MOUSEBUTTONDOWN)
      {
        if (event.button.button == SDL_BUTTON_LEFT)
        {
          if (!isMenu && !win)
          {
            Mix_PlayChannel(0, fx, 0);
            shooting = true;
          }
        }
        break;
      }
    }

    clear(levels[selectedLevel].skyColor);

    std::string currentLevel = "menu_" + std::to_string(selectedLevel + 1);

    if (!isMenu)
    {
      draw_floor(levels[selectedLevel].floorColor);
      r.render(shooting, levels[selectedLevel], selectedLevel);
      if (win_level && option == 0)
        ImageLoader::render(renderer, "win_menu_1", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
      else if (win_level && option == 1)
        ImageLoader::render(renderer, "win_menu_2", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
      else if (win)
      {
        ImageLoader::render(renderer, "win", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
      }
    }
    else
    {
      ImageLoader::render(renderer, currentLevel, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
      win = false;
      r.player.x = BLOCK + BLOCK / 2;
      r.player.y = BLOCK + BLOCK / 2;
      r.player.a = M_PI / 4.0f;
      r.player.fov = M_PI / 3.0f;
      if (!playMusic)
      {
        playMusic = true;
        Mix_PlayMusic(menu, -1);
      }
    }

    // render
    SDL_RenderPresent(renderer);
    frameTick = SDL_GetTicks() - frameStart;
    screenTitle = "Proyecto 2 - Raycasting / FPS: " + std::to_string(1000.0 / frameTick);
    SDL_SetWindowTitle(window, screenTitle.c_str());
  }

  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}