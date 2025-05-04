#include "Dot.h"
#include "Character.h"
#include <utility>  // Để sử dụng std::pair



Dot** dotBee=nullptr;


 Dot::Dot(int x,int y,int typeE)
 {
     //Initialize the collision box
     mBox.x = x;
     mBox.y = y;
 	mBox.w = DOT_WIDTH;
 	mBox.h = DOT_HEIGHT;

     //Initialize the velocity
     mVelX = 0;
     mVelY = 0;
     lastDirection=rand()%3;
    walk=0;
    run=0;
    die=0;
    dead=0;
    goRight = 1;
    goLeft = 0;
    touch=touchX=touchY=0;
    attack=0;
    hurt=0;
    timeHurt=0;
    mHP=100;
    maxHP=100;
    dameSword=0;
    typeEnemy=typeE;

 }

 void Dot::handleEvent( SDL_Event& e ){
   //If a key was released
     if( e.type == SDL_KEYUP && e.key.repeat == 0 )
     {
         //Adjust the velocity
         switch( e.key.keysym.sym )
         {   case SDLK_LSHIFT: run=0; break;
             case SDLK_LEFT:  mVelX =0; break;
             case SDLK_RIGHT:   mVelX =0; break;
             //case SDLK_SPACE: attacking=0; break;
         }

     }

     //If a key was pressed
 	else if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
     {
         //Adjust the velocity
         switch( e.key.keysym.sym )
         {
             case SDLK_LEFT: mVelX -= DOT_VEL; break;
             case SDLK_RIGHT:  mVelX += DOT_VEL;  break;
             case SDLK_LSHIFT: run=1; break;
            case SDLK_SPACE: attacking=true; break;

         }

     }




      if(mVelX!=0 || mVelY!=0){walk=1;}
      else walk =0;

       goRight=0; goLeft =0;
      if(mVelX>0)goRight=1;
      else if(mVelX<0)goLeft=1;

 }

void Dot::updatePosition(std::pair<int,int>* position) {
    // Di chuyển bóng các vị trí còn lại về trước (đẩy lùi các bóng)
    for (int i = 19; i > 0; --i) {
        position[i] = position[i - 1];  // Đẩy lùi các bóng
    }

    // Cập nhật bóng số 0 là vị trí hiện tại của nhân vật
    position[0] = std::make_pair(mBox.x, mBox.y);  // Cập nhật bóng với vị trí hiện tại
}

void Dot::AiHandleEvent( Tile* tiles[]) {
            int moveCode = rand() % 600;

            if (moveCode >=20) {
                // Giữ nguyên hướng di chuyển
            } else {
                if (moveCode < 2)
                    lastDirection = (lastDirection + 2) % 4;
                else if (moveCode <= 10)
                    lastDirection = (lastDirection + 1) % 4;
                else if (moveCode <= 19)
                    lastDirection = (lastDirection - 1 + 4) % 4;

                switch (lastDirection) {
                    case 0: mVelX = DOT_VEL; mVelY = 0; break;
                    case 1: mVelX = 0; mVelY = -DOT_VEL; break;
                    case 2: mVelX = -DOT_VEL; mVelY = 0; break;
                    case 3: mVelX = 0; mVelY = DOT_VEL; break;
                }
            }

            touch = 0;
            move(tiles, 0.01);

            if(touchX){mVelX = -mVelX;}
            if(touchY){mVelY = -mVelY;}


            goUp = goDown = goRight = goLeft = 0;
            if (mVelX > 0) { goRight = 1; lastDirection = 0; }
            else if (mVelX < 0) { goLeft = 1; lastDirection = 2; }

            if (mVelY > 0) { goDown = 1; lastDirection = 3; }
            else if (mVelY < 0) { goUp = 1; lastDirection = 1; }

}


void Dot::move( Tile *tiles[], float timeStep )
 {
     if(typeEnemy==0)mVelY= (-DOT_VEL*2);
     //Move the dot left or right
     float i=(isRun()?2:1);
     float j=(isAttacking()?0.5:1); // nếu đang tấn công; tốc độ giảm đi; với các động vật có tốc độ thấp thì rõ ràng int (<1 )=0
     int X= mVelX *2 * timeStep *i*j;
     mBox.x += X;

     if(touchesWall( mBox, tiles )==2)mHP=(mHP+1);

    if( ( mBox.x < 0 ) || ( mBox.x + DOT_WIDTH > LEVEL_WIDTH ) || (touchesWall( mBox, tiles )==1) )
     {
         //move back
         mBox.x -= X;
         touchX=1;

     }
     else{touchX=0;}

     //Move the dot up or down
     int Y=mVelY * timeStep *i*j;
     mBox.y += Y;


      if( ( mBox.y < 0 ) || ( mBox.y + DOT_HEIGHT > LEVEL_HEIGHT ) || (touchesWall( mBox, tiles )==1) )
     {
         //move back
         mBox.y -=Y;
         touchY=1;
         mHP-=1;
         late +=2;
     }
     else{touchY=0;if(late)late--;}
     if(touchX && touchY)touch=1;
     else{touch=0;}
    if(mHP<=0 || late >SCREEN_HEIGHT/2 +50){dead=1;}
    if(die)late=0;


     if(mBox.y<=500){
    mBox.y=(15508);
    }
 }



bool Dot::attackEnemy(Dot* dotEnemy[], int numEnemies, int attackRange,bool inThisFrame) {      // hàm này sẽ trừ máu kẻ địch; định sự đau và cuồng nộ

static int timeMana=0;
        timeMana+=1;
        if(timeMana%100==0){
            mana++;                    // bản thân mana sẽ tăng dần
            if(run){mana-=2;}          // kĩ năng thì tiêu tốn
            if(mana>maxMana)mana=maxMana;
            if(mana<0)mana=0;
            if(mana==0)run=0;
        }
        if(timeMana%500==0)mHP+=1;            // bản thân máu cũng sẽ hồi mỗi giây
        if(mHP>maxHP)mHP=maxHP;
        if(timeMana>10000)timeMana=0;

    ////////////////////////////////
    ///////////////////////////////
    bool damage=0;
        if (isAttacking() && inThisFrame) {
            for (int i = 0; i < numEnemies; ++i) {


                if (checkDistance((float)GetX(),(float)GetY(),(float)dotEnemy[i]->GetX(),(float)dotEnemy[i]->GetY(),attackRange)) {
                        if(dotEnemy[i]->typeEnemy==4) {damage=true;SetMaxHP(GetMaxHP()+50);dotEnemy[i]->SetHP(-100);}
                }
            }
        }
       return damage;
    }


 void Dot::setCamera( SDL_Rect& camera )
 {
 	//Center the camera over the dot
 	camera.x = ( mBox.x + DOT_WIDTH / 2 ) - SCREEN_WIDTH / 2;
 	camera.y = ( mBox.y -late + DOT_HEIGHT / 2 ) - SCREEN_HEIGHT / 2 - 100;

 	//Keep the camera in bounds
 	if( camera.x < 0 )
 	{
 		camera.x = 0;
 	}
 	if( camera.y < 0 )
 	{
 		camera.y = 0;
 	}
 	if( camera.x > LEVEL_WIDTH - camera.w )
 	{
 		camera.x = LEVEL_WIDTH - camera.w;
 	}
 	if( camera.y > LEVEL_HEIGHT - camera.h )
 	{
 		camera.y = LEVEL_HEIGHT - camera.h;
 	}
 }


