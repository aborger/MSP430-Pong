import processing.serial.*;


// Serial
Serial myPort;                       // The serial port
int IMUval = 0;
boolean isMenu = true;


Paddle paddleLeft;
Paddle paddleRight;
Ball ball;
Menu menu;
Explode explode;

int scoreLeft = 0;
int scoreRight = 0;

int DIST_FROM_EDGE = 10;
int PADDLE_SPEED = 5;
int INITIAL_BALL_SPEED = 3;


void setup() {
  size(800, 600);
  
  println(Serial.list());
  
  String portName = Serial.list()[2];
  myPort = new Serial(this, portName, 9600);
  
  paddleLeft = new Paddle(15, height/2, 30,200);
  paddleRight = new Paddle(width-15, height/2, 30,200);
  ball = new Ball(width/2, height/2, 35);
  ball.speedX = 5;
  ball.speedY = random(INITIAL_BALL_SPEED * -1, INITIAL_BALL_SPEED);
}

void draw() {
  background(0);
  
  if (scoreLeft == 10 || scoreRight == 10) {
    isMenu = true;
  }
  
  if (isMenu) {
    menu = new Menu();
    myPort.write(0x01);
  } else {
  

  // Paddle out of bounds
  if (paddleLeft.bottom() > height) {
    paddleLeft.y = height-paddleLeft.h/2;
  }

  if (paddleLeft.top() < 0) {
    paddleLeft.y = paddleLeft.h/2;
  }
    
  if (paddleRight.bottom() > height) {
    paddleRight.y = height-paddleRight.h/2;
  }

  if (paddleRight.top() < 0) {
    paddleRight.y = paddleRight.h/2;
  }
  
  
  // Ball reflection
  
  if ( ball.left() < paddleLeft.right() && ball.y > paddleLeft.top() && ball.y < paddleLeft.bottom()){
    ball.speedX = -ball.speedX;
    if (ball.speedX > 0) {
      ball.speedX += 1;
    }
    else {
      ball.speedX -= 1;
    }
    if (ball.speedY > 0) {
      ball.speedY += 1;
    }
    else {
      ball.speedY -= 1;
    }
  }

  if ( ball.right() > paddleRight.left() && ball.y > paddleRight.top() && ball.y < paddleRight.bottom()) {
    ball.speedX = -ball.speedX;
    if (ball.speedX > 0) {
      ball.speedX += 1;
    }
    else {
      ball.speedX -= 1;
    }
    if (ball.speedY > 0) {
      ball.speedY += 1;
    }
    else {
      ball.speedY -= 1;
    }
  }
  
  // Goal scored
  if (ball.right() > width) {
    scoreLeft = scoreLeft + 1;
    ball.x = width/2;
    ball.y = height/2;
    ball.speedX = -1 * INITIAL_BALL_SPEED;
    ball.speedY = random(INITIAL_BALL_SPEED * -1, INITIAL_BALL_SPEED);
  }
  if (ball.left() < 0) {
    scoreRight = scoreRight + 1;
    ball.x = width/2;
    ball.y = height/2;
    ball.speedX = INITIAL_BALL_SPEED;
    ball.speedY = random(INITIAL_BALL_SPEED * -1, INITIAL_BALL_SPEED);
  }
  
  textSize(40);
  textAlign(CENTER);
  
  text(scoreRight, width/2+30, 30); // Right side score
  text(scoreLeft, width/2-30, 30); // Left side score
  
  
  paddleLeft.move();
  paddleLeft.display();
  paddleRight.move();
  paddleRight.display();
  ball.move();
  ball.display();
  }
}


void serialEvent(Serial myPort) {
  int inByte = myPort.read();
  
  if (isMenu) {
    if (inByte == 1) {
      println("Not Ready");
    } else if (inByte == 2) {
      println("Player is ready");
      isMenu = false;
      myPort.write(0x02);
    }
  }
  else {
    IMUval = inByte - 65;
    println(IMUval);
    paddleRight.y += IMUval * -0.1;
    myPort.clear();
    myPort.write(0x02);
  }
  
  
  
}

void keyPressed(){
  if(keyCode == UP){
    //paddleRight.speedY = PADDLE_SPEED * -1;
  }
  if(keyCode == DOWN){
    //paddleRight.speedY = PADDLE_SPEED;
  }
  if(key == 'w'){
    paddleLeft.speedY = PADDLE_SPEED * -1;
  }
  if(key == 's'){
    paddleLeft.speedY = PADDLE_SPEED;
  }
}

void keyReleased(){
  if(keyCode == UP){
    //paddleRight.speedY = 0;
  }
  if(keyCode == DOWN){
    //paddleRight.speedY = 0;
  }
  if(key == 'w'){
    paddleLeft.speedY = 0;
  }
  if(key == 's'){
    paddleLeft.speedY = 0;
  }
}
