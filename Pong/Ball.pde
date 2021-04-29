class Ball { 
  float x;
  float y;
  float speedX;
  float speedY;
  float diameter;
  color c;
  
  Ball(float tempX, float tempY, float tempDiameter) {
    x = tempX;
    y = tempY;
    diameter = tempDiameter;
    speedX = 0;
    speedY = 0;
    c = (225); 
  }

  void move() {
    // Add speed to location
    y = y + speedY;
    x = x + speedX;
    
    if (ball.right() > width) { //if stuff between () is true, execute code between {}
      ball.speedX = -ball.speedX;
    }
    if (ball.left() < 0) {
      ball.speedX = -ball.speedX;
    }

    if (ball.bottom() > height) {
      ball.speedY = -ball.speedY;
    }

    if (ball.top() < 0) {
      ball.speedY = -ball.speedY;
    }
  }
  
  void display() {
    fill(c);
    ellipse(x,y,diameter,diameter);
  }

  //functions to help with collision detection
  float left(){
    return x-diameter/2;
  }
  float right(){
    return x+diameter/2;
  }
  float top(){
    return y-diameter/2;
  }
  float bottom(){
    return y+diameter/2;
  }
}
