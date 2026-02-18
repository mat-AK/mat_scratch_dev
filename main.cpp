#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sddl.h>
using namespace std;

struct Sprite{
    string name;
    float x, y;
    float scale;
    float rotation;
    bool isVisable;
    int r, g, b;
    float penSize;
    bool isPenDown;
};

vector<Sprite> allSprites;

void saveProject (string file){
    ofstream outFile(file);
    if (!outFile.is_open()){
        cout<<"Error: Could not open file for saving!"<<endl;
        return;
    }
    for(int i = 0; i<allSprites.size(); i++){
        outFile<<allSprites[i].name<<" "<<allSprites[i].x<<" "<<allSprites[i].y<<" "<<allSprites[i].scale<<" "<<allSprites[i].rotation<<" "<<allSprites[i].r<<" "<<allSprites[i].g<<" "<<allSprites[i].b<<" "<<allSprites[i].penSize<<endl;
    }
    outFile.close();
    cout<<"Project saved to "<<file<<endl;
}

void loadProject(string file){
    ifstream inFile (file);
    if(!inFile.is_open()){
        cout<<"Error: file is not found!"<<endl;
        return;
    }
    allSprites.clear();
    Sprite temp;
    while(inFile>>temp.name>>temp.x>>temp.y>>temp.scale>>temp.rotation>>temp.r>>temp.g>>temp.b>>temp.penSize){
        temp.isVisable = true;
        temp.isPenDown = false;
        allSprites.push_back(temp);
    }
    inFile.close();
    cout<<"Project loaded succesfully. count: "<<allSprites.size()<<endl;
}

void addNewSprite(string name, float x, float y){
    Sprite newSprite;
    newSprite.name = name;
    newSprite.x = x;
    newSprite.y = y;
    newSprite.scale = 1.0f;
    newSprite.rotation = 0.0f;
    newSprite.r = 0;
    newSprite.g = 0;
    newSprite.b = 0;
    newSprite.penSize = 1.0f;
    newSprite.isVisable = true;
    allSprites.push_back(newSprite);
}

int main(int argc, char* argv[]){
    addNewSprite("Cat",100,150);
    addNewSprite("Dog",-50,20);
    saveProject("p");
    return 0;
}
