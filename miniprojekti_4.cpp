#include <SDL2/SDL.h>
#include <box2d/box2d.h>
#include <iostream>
#include <array>

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }
    
    SDL_Window* window = SDL_CreateWindow("miniprojekti_4",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          1280,
                                          720,
                                          SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    
    if (window == nullptr) {
        std::cout << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }
    
    auto clean_up = [window]() {
        SDL_DestroyWindow(window);
        SDL_Quit();
    };
    
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == nullptr) {
        std::cout << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        clean_up();
        return EXIT_FAILURE;
    }
    
    b2World world(b2Vec2(0.0f, -9.81f));
    
    {
        b2BodyDef ground_body_def;
        ground_body_def.position.Set(0.0f, -10.0f);
        
        b2PolygonShape ground_shape;
        ground_shape.SetAsBox(10.0f, 1.0f);
        
        b2Body* ground_body = world.CreateBody(&ground_body_def);
        ground_body->CreateFixture(&ground_shape, 0.0f);
    }

    auto spawn_box = [&world](){
        b2BodyDef body_def;
        body_def.type = b2_dynamicBody;
        body_def.position.Set(0.0f, 10.0f);
        body_def.angle = 45.0f;
        
        b2PolygonShape shape;
        shape.SetAsBox(1.0f, 1.0f);
        
        b2FixtureDef fixture_def;
        fixture_def.shape = &shape;
        fixture_def.density = 1.0f;
        fixture_def.friction = 0.3f;
        
        b2Body* body = world.CreateBody(&body_def);
        body->CreateFixture(&fixture_def);
    };

    auto draw = [renderer, &world]() {
        int pixel_width;
        int pixel_height;
        SDL_GetRendererOutputSize(renderer, &pixel_width, &pixel_height);
        
        float view_size = 25.0f;
        float aspect_ratio = static_cast<float>(pixel_width) / pixel_height;
        
        float scale;
        if (aspect_ratio > 1) {
            scale = pixel_height / view_size;
        }
        else {
            scale = pixel_width / view_size;
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        
        for (b2Body* body = world.GetBodyList(); body != nullptr; body = body->GetNext()) {
            
            for (b2Fixture* fixture = body->GetFixtureList(); fixture != nullptr; fixture = fixture->GetNext()) {
                if (fixture->GetShape()->GetType() == b2Shape::e_polygon) {
                    
                    b2PolygonShape* shape = static_cast<b2PolygonShape*>(fixture->GetShape());
                    
                    const int vertex_count = shape->m_count;
                    const int point_count = vertex_count + 1;
                    std::array<SDL_Point, b2_maxPolygonVertices + 1> points;
                    
                    for (int i = 0; i < vertex_count; i++) {
                        points[i].x = static_cast<int>(body->GetWorldPoint(shape->m_vertices[i]).x * scale);
                        points[i].y = -static_cast<int>(body->GetWorldPoint(shape->m_vertices[i]).y * scale);
                        
                        points[i].x += pixel_width / 2;
                        points[i].y += pixel_height / 2;
                    }
                    
                    if (point_count > 1) {
                        points[point_count - 1] = points[0];
                    }
                    
                    SDL_RenderDrawLines(renderer, points.data(), point_count);
                }
            }
        }
        SDL_RenderPresent(renderer);
    };
    
    bool quit = false;
    
    using TimePoint = Uint64;
    using Duration = Uint64;
    
    const Duration TIMESTEP = 1;
    const Duration BOX_SPAWN_INTERVAL = 1000;
    
    Duration since_simulate = 0;
    TimePoint previous_render = SDL_GetTicks64();
    
    Duration now = 0;
    TimePoint next_box_spawn = SDL_GetTicks64();
    
    while (!quit)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
        
        TimePoint render_time = SDL_GetTicks64();
        Duration since_render = render_time - previous_render;
        previous_render = render_time;
        
        since_simulate += since_render;
        while (since_simulate >= TIMESTEP) {
            if (now >= next_box_spawn) {
                next_box_spawn += BOX_SPAWN_INTERVAL;
                spawn_box();
            }
            
            world.Step(static_cast<float>(TIMESTEP) / 1000.0f, 6, 2);
            now += TIMESTEP;
            since_simulate -= TIMESTEP;
        }
        
        draw();
        SDL_Delay(TIMESTEP);
    }
    
    clean_up();
    return EXIT_SUCCESS;
}
