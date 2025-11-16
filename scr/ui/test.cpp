#include "C:\Users\joyce\OneDrive\桌面\UNO\colour_render.h"

#include <iostream>
#include <string>

using namespace std;

int main() {
    RenderCard render; 
    cout<< "PC1:Theift. Remaining cards: 3"<< endl;
    cout<< "PC2:Lucky Star. Remaining cards: 2"<< endl;
    cout<< "PC3:Collector. Remaining cards: 4"<< endl;
    cout<< "You:Defender. Skill used?(Y/N): Y"<< endl;
    cout<< "The top card is:"<< endl;
    render.render_card("RED", "ACTION", 12);
    cout<< "Your card(s):"<<endl;
    render.render_card("BLUE", "NUMBER", 1);
    render.render_card("FLASH","ACTION_NOCOLOUR",1000);
    cout<< "Please input index to play the card."<< endl;
    return 0;
}