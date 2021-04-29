class Explode {
  int numParticles = 10;
  Particle[] particles;
  boolean j = true;
  
  Explode() {
    particles = new Particle[numParticles];
    for (int i = 0; i < numParticles; i++) {
      particles[i].display();
    }
  }
  
}

class Particle {
  float x = width/2;
  float y = height/2;
  float speedX;
  float speedY;
  float diameter = 10;
  
  Particle(float tempSpeedX, float tempSpeedY) {
    speedX = tempSpeedX;
    speedY = tempSpeedY;
  }
  
  void display() {
    fill(255);
    ellipse(x,y,diameter,diameter);
    x += speedX;
    y += speedY;
  }
}
