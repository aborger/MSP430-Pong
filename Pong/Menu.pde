class Menu {
  int h = 75;
  int w = 350;
  Menu() {
    fill(255);
    rect(width/2 - w/2,height/2 - h/2,w,h);
    textSize(40);
    fill(0);
    textAlign(CENTER);
    text("Play", width/2, height/2 + h/5);
  }
    
}
