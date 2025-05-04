
 // Code được tham khảo từ lazyfoo.net

 #include <SDL.h>
 #include <SDL_image.h>
 #include <SDL_ttf.h>
 #include <SDL_mixer.h>
 #include <stdio.h>
 #include <string>
 #include <fstream>
 #include <sstream>
 #include <iomanip>
 #include <cmath>


 #include "LTimer.h"
 #include "Bee.h"


 //Starts up SDL and creates window
 bool init();

 //Loads media
 bool loadMedia( Tile* tiles[] );
 //Frees media and shuts down SDL
 void close(Tile* tiles[],bool loadMap,
            bee**& Bee, Dot**& dotBee, int& numBee );

 //////////// bộ load
 int getLevel();          // hàm để load level đã chơi
 void RenderOpen(int& GameStarted);                //vẽ menu
 void handleOpenGame(SDL_Event &e, bool &quit, int& level, int& GameStarted);    // sụ kiện menu
 bool loadGame(int level,Tile* tiles[]);                      // load theo màn chơi
 void GetBegin(const std::string& filename);          // khởi tạo vị trí và các chỉ số nhan vật

 bee** createBeesFromFile (const std:: string& filename, int& numBee, Dot**& dotBee);

 //////////// bộ xử lý game
 int handleEvent(SDL_Event &e, Character &character,  bool &quit);          // nhận vật di chuyển và sự kiện khác
 void AiHandle(SDL_Event& e, Tile* tileSet[],std:: pair<int,int>* position = nullptr);   // điều kiển enemy
 void handleRain(Uint32 &frameStart);        // thời tiết mưa
 void EnemyAttack();   // kẻ địch phản ứng
 void handleQ(SDL_Event& e); // sự kiện mở cửa sổ chỉ dẫn
 void renderQ();  // vẽ cửa sổ chỉ dẫn theo yêu cầu;

 /////////// bộ thoát

 bool renderPass(Dot** dotEnemy, int numEnemies);

 void freeBees(bee**& Bee, Dot**& dotBee, int& numBee);
 void closeMap(Tile* tiles[],
               bee**& Bee, Dot**& dotBee, int& numBee
               ); /////// ta dùng để xóa map và các nhân vật; để load màn chơi mới

 void saveLevel(const int& Level);



 ////////////////////////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////////////////////////

    bee** Bee;

    //int mana;
    //int EXP;

    int Level=getLevel();
    bool isGameLoaded = false;  // Biến flag kiểm tra xem game đã được load chưa
    bool inMenu=false;       // trạng thái trở về menu
    int handbook = 0;            // trạng thái của hướng dẫn
 /////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////


int rank=10000;       // tôi mặc định đây tương ứng với thời gian chúng ta sẽ phải vượt qua để có thể qua màn chơi

int score =0;



 bool init()
 {
 	//Initialization flag
 	bool success = true;

 	//Initialize SDL
 	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
 	{
 		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
 		success = false;
 	}
 	else
 	{
 		//Set texture filtering to linear
 		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
 		{
 			printf( "Warning: Linear texture filtering not enabled!" );
 		}

 		//Create window
 		gWindow = SDL_CreateWindow( "The Grass", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
 		if( gWindow == NULL )
 		{
 			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
 			success = false;
 		}
 		else
 		{
 			//Create renderer for window
 			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
 			if( gRenderer == NULL )
 			{
 				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
 				success = false;
 			}
 			else
 			{
 				//Initialize renderer color
 				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

 				//Initialize PNG loading
 				int imgFlags = IMG_INIT_PNG;
 				if( !( IMG_Init( imgFlags ) & imgFlags ) )
 				{
 					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
 					success = false;
 				}
 				//Initialize SDL_ttf
				if( TTF_Init() == -1 )
				{
					printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
					success = false;
				}
				//Initialize SDL_mixer
				int flags = MIX_INIT_MP3;
                if ((Mix_Init(flags) & flags) != flags) {
                    printf("Mix_Init failed: %s\n", Mix_GetError());
                    SDL_Quit();
                    return false;
                }
				if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
				{
					printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
					success = false;
				}
 			}
 		}
 	}

 	return success;
 }

 bool loadMedia( )
 {
 	//Loading success flag
 	bool success = true;
 	if (!OpenTexture.loadFromFile("image/Game_Open.png")) {
        printf("Failed to load OP!\n");
        success = false;
    }
    if (!OpenTexture1.loadFromFile("image/Game_Open1.png")) {
        printf("Failed to load OP1!\n");
        success = false;
    }


 	// Load texture1 từ file
    if (!texture1.loadFromFile("image/O1.png")) {
        printf("Failed to load texture1!\n");
        success = false;
    }

    // Load texture2 từ file
    if (!texture2.loadFromFile("image/O2.png")) {
        printf("Failed to load texture2!\n");
        success = false;
    }

    // Load texture3 từ file
    if (!texture3.loadFromFile("image/O3.png")) {
        printf("Failed to load texture3!\n");
        success = false;
    }

    // Load texture4 từ file
    if (!texture4.loadFromFile("image/O4.png")) {
        printf("Failed to load texture4!\n");
        success = false;
    }
    ////////////////////////// Load hand book
    if (!HandBook1Texture.loadFromFile("image/Q1.png")) {
        printf("Failed to load hand book 1!\n");
        success = false;
    }
    if (!HandBook2Texture.loadFromFile("image/Q2.png")) {
        printf("Failed to load hand book 2!\n");
        success = false;
    }


 	//Open the font
	gFont = TTF_OpenFont( "font/VNLUCIDA.TTF", 12 );
	if( gFont == NULL )
	{
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
		success = false;
	}
	if (!Q.loadFromRenderedText("Press Q to view the instructions, press X at the top right corner of the screen to pause or exit the game.",nameColor)) {
        printf("Failed to load Q !\n");
        success = false;
    }

 	if( !gPauseTexture.loadFromFile( "image/Game_Pause.png" ) )
 	{
 		printf( "Failed to load tile Pause texture!\n" );
 		success = false;
 	}
 	if( !gOverTexture.loadFromFile( "image/Game_Over.png" ) )
 	{
 		printf( "Failed to load tile Over texture!\n" );
 		success = false;
 	}
 	if( !gPassTexture.loadFromFile( "image/Game_Pass.png" ) )
 	{
 		printf( "Failed to load tile Pass texture!\n" );
 		success = false;
 	}
 	else{
        gPassTexture.setBlendMode(SDL_BLENDMODE_BLEND);
        gPassTexture.setAlpha(0);
 	}
 	if( !gRainTexture.loadFromFile( "image/rain.png" ,255,255,255) )
 	{
 		printf( "Failed to load tile rain texture!\n" );
 		success = false;
 	}
 	else{
        gRainTexture.setBlendMode(SDL_BLENDMODE_BLEND);
        gRainTexture.setAlpha(0);
 	}


 	//Load dot Charracter & enemy
 	if( !gCharacter.loadMedia() )
 	{
 		printf( "Failed to load character!\n" );
 		success = false;
 	}

 	//Load tile texture
 	for (int i = 0; i < TOTAL_TILE_SPRITES ; ++i) {
        std::ostringstream filename;
        filename << "image/objects/1 Tiles/"<<std::setw(3) << std::setfill('0') << (i + 1) << ".png";  // Tạo tên file từ 001.png đến 104.png

        // Gọi hàm loadFromFile cho từng texture
        if (!gTileTexture[i].loadFromFile(filename.str())) {
            printf("Failed to load texture: %s\n", filename.str().c_str());
            success = false;
        }
        else{gTileTexture[i].setBlendMode(SDL_BLENDMODE_BLEND);}
    }

 	gGo = Mix_LoadMUS( "audio/go.mp3" );
	if( gGo == NULL )
	{
		printf( "Failed to load go music! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	gRun = Mix_LoadMUS( "audio/run.mp3" );
	if( gRun == NULL )
	{
		printf( "Failed to load run music! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	gSword = Mix_LoadWAV( "audio/sword_attack.mp3" );
	if( gSword == NULL )
	{
		printf( "Failed to load sword sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	open=Mix_LoadMUS( "audio/open.mp3" );game1=Mix_LoadMUS( "audio/game1.mp3" );game2=Mix_LoadMUS( "audio/game2.mp3" );
	if( gSword == NULL||game1==NULL || game2==NULL )
	{
		printf( "Failed to load music game! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}


 	return success;
 }

 bool loadGame(int level,Tile* tiles[]){
    bool success =true;
if(level==1){

    GetBegin("save_game/Begin1.txt");
    Bee=createBeesFromFile("save_game/Bee1.txt",numBee,dotBee);
    //Load tile map
        if( !setTiles( tiles,1 ) )
        {
            printf( "Failed to load tile set!\n" );
            success = false;
        }
    }
else if(level==2){
    GetBegin("save_game/Begin2.txt");
    Bee=createBeesFromFile("save_game/Bee2.txt",numBee,dotBee);
    //Load tile map
        if( !setTiles( tiles,2 ) )
        {
            printf( "Failed to load tile set!\n" );
            success = false;
        }
    }
else if(level==3){
    GetBegin("save_game/Begin3.txt");
    Bee=createBeesFromFile("save_game/Bee3.txt",numBee,dotBee);
    //Load tile map
        if( !setTiles( tiles,3 ) )
        {
            printf( "Failed to load tile set!\n" );
            success = false;
        }
    }
else{
    GetBegin("save_game/Begin1.txt");
    Bee=createBeesFromFile("save_game/Bee1.txt",numBee,dotBee);
    //Load tile map
        if( !setTiles( tiles,1 ) )
        {
            printf( "Failed to load tile set!\n" );
            success = false;
        }
}
return success;
}


 void close(Tile* tiles[],bool loadMap,
            bee**& Bee, Dot**& dotBee, int& numBee )
             {
    if(loadMap)closeMap(tiles,Bee,dotBee,numBee);
    // Giải phóng các tài nguyên khác (như texture, âm thanh)
    gPauseTexture.free();
    gPassTexture.free();
    gOverTexture.free();
    gRainTexture.free();

    gCharacter.free();
    OpenTexture.free();
    OpenTexture1.free();

    texture1.free();
    texture2.free();
    texture3.free();
    texture4.free();

    HandBook1Texture.free();
    HandBook2Texture.free();

    Q.free();

    // Giải phóng bộ nhớ cho các đối tượng khác như gTileTexture, gFont, v.v.
    for (int i = 0; i < TOTAL_TILE_SPRITES; ++i) {
        gTileTexture[i].free();
    }

    // Giải phóng các tài nguyên SDL
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = nullptr;
    gRenderer = nullptr;
    TTF_CloseFont(gFont);
    gFont = nullptr;
    Mix_FreeChunk(gSword);
    gSword = nullptr;
    Mix_FreeMusic(gRun);
    Mix_FreeMusic(gGo);
    Mix_FreeMusic(open);
    Mix_FreeMusic(game1);
    Mix_FreeMusic(game2);
    gRun = nullptr;
    gGo = nullptr;

    // Quit SDL subsystems
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

 void closeMap(Tile* tiles[],

               bee**& Bee, Dot**& dotBee, int& numBee)
               {
   // Giải phóng bộ nhớ cho từng Tile trong mảng

        for (int i = 0; i < TOTAL_TILES; ++i) {
            if (tiles[i] != nullptr) {
                delete tiles[i];  // Giải phóng bộ nhớ của Tile[i]
                tiles[i] = nullptr;  // Đặt lại con trỏ về nullptr để tránh sử dụng lại
            }
        }


    freeBees(Bee, dotBee, numBee);

 }




//////////////////////////////////////////////////////////////////////////////////
int getLevel() {
    int level = 0;
    std::ifstream inputFile("save_game/level.txt");

    if (inputFile.is_open()) {
        inputFile >> level;
        inputFile.close();
    } else {
        printf("Cant open file level.txt to read");
    }

    return level;
}

void RenderOpen(int& GameStarted) {                            // lưu ý rằng gamestarted =2 mới tính là vào game
    if(GameStarted==0){
        OpenTexture.render(1,1);
    }
    else if(GameStarted==1){
        OpenTexture1.render(0,0);
        if (Level >= 1) {
            texture1.render(80, 100);
        }
        if (Level >= 2) {
            texture2.render(180, 100);
        }
        if (Level >= 3) {
            texture3.render(280, 100);
        }
        if (Level >= 4) {
            texture4.render(380, 100);
        }
    }
}

void handleOpenGame(SDL_Event &e, bool &quit, int& level, int& GameStarted) {
    if (GameStarted == 0) {
        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
            quit = true;
        } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN && e.key.repeat == 0) {
            GameStarted = 1;
        }
    } else if (GameStarted == 1) {
        switch (e.type) {
            case SDL_QUIT:
                GameStarted = 0;
                break; // Nếu người dùng nhấn nút thoát

            case SDL_KEYDOWN: // Xử lý sự kiện khi nhấn phím
                if (e.key.repeat == 0) {  // Kiểm tra không phải sự kiện phím lặp lại
                    if (e.key.keysym.sym == SDLK_ESCAPE) {
                        GameStarted = 0;
                    } else if ((e.key.keysym.sym == SDLK_1) && (Level>=1)) {
                        level = 1;
                        GameStarted = 2;
                    } else if ((e.key.keysym.sym == SDLK_2)&&(Level>=2)) {
                        level = 2;
                        GameStarted = 2;
                    } else if (e.key.keysym.sym == SDLK_3&&(Level>=3)) {
                        level = 3;
                        GameStarted = 2;
                    } else if (e.key.keysym.sym == SDLK_4&&(Level>=4)) {
                        level = 4;
                        GameStarted = 2;
                    } else if (e.key.keysym.sym == SDLK_SPACE) {
                        level = Level;
                        GameStarted = 2;
                    }
                }
                break;

            default:
                break; // Không làm gì nếu là sự kiện khác
        }
    }
}

 int handleEvent(SDL_Event &e, Character &character,  bool &quit) {        // ta dùng int để check xem sự kiện có phải sự kiện thóat, hoặc về menu không
    static bool waitingForQuit = false;
    // Xử lý sự kiện của Character
    character.handleEvent(e);

    // Xử lý sự kiện thoát
    if (e.type == SDL_QUIT) {
        waitingForQuit = true;
    }

    if (waitingForQuit) {
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_y) {
                printf("Exiting game...\n");
                quit = true;
            } else if (e.key.keysym.sym == SDLK_n) {
                printf("Returning to game...\n");
                waitingForQuit = false;
            }
            else if(e.key.keysym.sym==SDLK_ESCAPE){
                waitingForQuit=false;
            return 2;
           }
        }
    }
    return waitingForQuit;
}

 bool renderPass(Dot** dotEnemy, int numEnemies) {
    bool render = true;

    // Kiểm tra xem có kẻ địch nào còn sống không
    for (int i = 0; i < numEnemies; i++) {
        if (dotEnemy[i]->isDie() != 1) {
            render = false;
            break;
        }
    }


    return render;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////


bee** createBeesFromFile(const std::string& filename, int& numBee, Dot**& dotBee){
    std::ifstream file(filename);
    if (!file) {
        printf("Không thể mở file!\n");
        return nullptr;
    }
    file >> numBee;
    bee** Bee = new bee*[numBee];
    dotBee = new Dot*[numBee];  // Cấp phát mảng Dot* tương ứng
    int x, y;
    for (int i = 0; i < numBee; ++i) {
        file >> x >> y;
        Bee[i] = new bee(x, y);
        if (i == 0) Bee[i]->loadMedia();
        dotBee[i] = Bee[i]->GetDot();
    }

    // Đóng file
    file.close();

    return Bee;

}

void freeBees(bee**& Bee, Dot**& dotBee, int& numBee) {
    //
    for (int i = 0; i < numBee; ++i) {
        if (Bee[i] != nullptr) {
            Bee[i]->free();
            delete Bee[i];
            Bee[i] = nullptr;
        }
    }

    if(numBee!=0)bee::freeStaticTextures();
    delete[] Bee;
    delete[] dotBee;

    // Đặt lại số lượng wolve
    numBee = 0;
}

void saveLevel(const int& Level) {
    std::ofstream outputFile("save_game/level.txt");

    if (outputFile.is_open()) {
        outputFile << Level;
        outputFile.close();
        printf("Level was saved: %d\n", Level);
    } else {
        printf("cant open file level.txt to write\n");
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////


void AiHandle( Tile* tileSet[] ) {


    for (int i = 0; i < numBee; i++) {
        dotBee[i]->AiHandleEvent( tileSet);
    }

}

void GetBegin(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        printf("Cant open file!\n");
        return;
    }

    int X, Y, HP, dame;
    file >> X >> Y >> HP >> dame;

    if (file.fail()) {
        printf("Read from file not sucess!\n");
        return;
    }

    dotCharacter->SetX(X);
    dotCharacter->SetY(Y);
    dotCharacter->SetMaxHP(HP);
    dotCharacter->SetHP(HP);
    dotCharacter->SetDameSword(dame);
    dotCharacter->SetMaxMana(100);
    dotCharacter->SetMana(100);
    dotCharacter->SetDead(0);               // loại bỏ các trạng thái xấu
    dotCharacter->SetDie(0);
    dotCharacter->SetTimeHurt(0);

}

///////////////////////////////////////////////////// ////các hàm trong game


void handleQ(SDL_Event& e) {
    if (e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_q) {
            handbook = 1;
            printf("Handbook set to 1\n");
        }

        if (e.key.keysym.sym == SDLK_RETURN) {
            if (handbook == 1) {
                handbook = 2;
                printf("Handbook set to 2\n");
            } else if (handbook == 2) {
                handbook = 0;
                printf("Handbook reset to 0\n");
            }
        }
    }
}

void renderQ(){
if(handbook==1)HandBook1Texture.render(0,0);
else if(handbook==2)HandBook2Texture.render(0,0);

}

//////////////////////////



int main(int argc, char* args[])
{
    srand(static_cast<unsigned int>(time(0)));

    // Start up SDL and create window
    if (!init())
    {
        printf("Failed to initialize!\n");
    }
    else
    {
        // The level tiles
        Tile* tileSet[TOTAL_TILES];
        static int level ;

        // Load media
        if (!loadMedia())
        {
            printf("Failed to load media!\n");
        }
        else
        {
            // Main loop flag
            bool quit = false;

            // Event handler
            SDL_Event e;

            // Keeps track of time between steps
            LTimer stepTimer;

            // Level camera
            SDL_Rect camera = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

            //std::pair<int, int>* position = createPositionArray(20);

            // Calculate the start time
            Uint32 frameStart;
            int frameTime;
///////////////////////////////////////////////////////////////////////////////////////

            while (!quit)
            {
                frameStart = SDL_GetTicks();  // Get the start time for the frame

                static bool play_open_music=false;
                static int GameStarted = 0;
                if (GameStarted != 2)
                {
                    if(!play_open_music){Mix_PlayMusic(open, -1);play_open_music=true;}
                    inMenu=false;         // không có lệnh về menu nữa
                    RenderOpen(GameStarted);
                    renderQ();
                    while (SDL_PollEvent(&e) != 0)
                    {
                        handleOpenGame(e, quit, level, GameStarted); ////////// mở đầu game; trả về gamestarted là 1 nếu ta bấm màn chơi
                        handleEvent(e, gCharacter, quit);
                        handleQ(e); // sự kiện mở hướng dẫn
                    }
                    if(GameStarted == 2){play_open_music=false;
                    Mix_HaltMusic();}

                }


                else
                {

                    if (!isGameLoaded)  // Chỉ load game một lần duy nhất và load sau khi gameStarted , load theo level màn chơi
                    {
                        if (!loadGame(level, tileSet))
                        {
                            quit = true;
                            break;  // Dừng game nếu không thể load game
                        }
                        isGameLoaded = true;  // Đánh dấu là game đã được load
                    }

                    else
                    {
                        static bool wait_for_quit = 0;  /////////////// nếu game đã bắt đầu

                        // Handle events on queue
                        static int renderP = 0; // chưa in pass lần nào
                        while (SDL_PollEvent(&e) != 0)
                        {
                            int handleE = handleEvent(e, gCharacter, quit);        // trả về 0: không làm gì; 1 là mở cửa sổ pause; 2 là về menu
                            if(handleE == 1)wait_for_quit = 1;
                            else if(handleE == 2){ inMenu=true; wait_for_quit=0;  }
                            else{wait_for_quit=0;}

                            handleQ(e);
                        }

                        // Calculate time step
                        float timeStep = stepTimer.getTicks() / 1000.f;

                        // Move the character
                        if (wait_for_quit == 0)
                        {
                            gCharacter.move(tileSet, timeStep);
                            AiHandle(tileSet);
                        }

                        gCharacter.attackEnemy(dotBee, numBee,1);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    // Restart step timer
                        stepTimer.start();

                        gCharacter.setCamera(camera);

                        gCharacter.setBlendMode(SDL_BLENDMODE_BLEND);
                        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                        SDL_RenderClear(gRenderer);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                        // Render level
                        for (int i = 0; i < TOTAL_TILES; ++i)
                        {
                            tileSet[i]->render(camera);
                        }

                        for (int i = 0; i < numBee; i++)
                        {
                            Bee[i]->render(camera);
                        }

                        gCharacter.render(camera);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                        score++;
                        dotCharacter->setVel(100* (1+score*10/rank));
                        if (score>=rank){                     // nếu ko mang về hơn 50 mật thì nhiệm vụ thất bại
                                if(gCharacter.GetHP()>50)
                                {score=rank;
                                    renderP++;
                                    gPassTexture.render(0,0);
                                    gPassTexture.setAlpha((renderP < 127) ? renderP * 2 : 255);
                                }
                                else dotCharacter->SetDead(1);
                        }


                        if(gCharacter.isDead()){
                            renderP++;
                            gOverTexture.render(0,0);
                            gOverTexture.setAlpha((renderP < 127) ? renderP * 2 : 255);

                        }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                        if (renderP >= 300  || inMenu  )             // nếu vượt qua màn hoặc có lệnh về menu
                        {
                            //quit = true;
                            if(!inMenu && !gCharacter.isDie())Level++;
                            if(Level>MaxLevel)Level=MaxLevel;  // tăng số màn chơi
                            GameStarted=1;
                            if((GameStarted==1) && isGameLoaded){closeMap(tileSet,Bee,dotBee,numBee);isGameLoaded=false;} // nếu quay trở lại menu; xóa màn chơi cũ đi
                            wait_for_quit=false;
                            renderP=0;
                            score=0;
                            gPassTexture.setAlpha(0);
                            Mix_HaltChannel(1);         // dừng phát âm thanh
                            Mix_HaltMusic();

                        }
                        if (wait_for_quit)
                        {
                            gPauseTexture.render(0, 0);
                        }
                        if(handbook==0)Q.render(0,700-Q.getHeight()-10);
                        else renderQ();
                    }
                }
/////////////////////////////////////////////////////////////////////////////////////////////
                // Update screen
                SDL_RenderPresent(gRenderer);

                // FPS
                frameTime = SDL_GetTicks() - frameStart;

                if (frameDelay > frameTime)
                {
                    SDL_Delay(frameDelay - frameTime); // Delay to maintain the FPS target
                }
                if(quit==true){saveLevel(Level);}   // lưu lại màn chơi

            }

            // Free resources and close SDL

            close(tileSet,isGameLoaded,
                  Bee,dotBee,numBee);
        }
    }

    return 0;
}







