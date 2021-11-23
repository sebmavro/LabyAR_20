#include "modelisation/OpenGL.h"
#include "analyse/EdgeDetection.h"
#include "modelisation/Transformation.h"
#include "physics/AngleModel.h"
#include "physics/CollisionDetection.h"
#include "windows.h"
#include <string.h>
#include <iostream>
#include <vector>
#include <locale>
#include <sstream>
#include <conio.h>
#include <chrono>
#include <opencv2/highgui/highgui_c.h>
#include <WS2tcpip.h>

#pragma comment(lib,"ws2_32.lib")

using namespace cv;
using namespace std;

CameraStream *cameraStream = nullptr;
OpenGL *window = nullptr;
AngleModel *angleModel = nullptr;
Ball *ball = nullptr;
__int64 diff = -1;

/// Pour récupérer les données de stress
HANDLE myHandle = nullptr;
unsigned int progress = 0;

/// Pour afficher les FPS
int frame = 0, myTime, timebase = 0;
double fps = 0.0;

/// Prototypes des fonctions de ce fichier
void loop(int);
void setupMaze();
DWORD WINAPI getMuseResult(LPVOID lpProgress);

int main(int argc, char** argv) {

    int64 tick = 0;
    bool timer = false;
    bool anaglyph;
    string name = "aMAZEd Calibration";
    ball = new Ball(0.5, 0.5, 0.02, 50);
    cameraStream = new CameraStream();
    namedWindow(name, WINDOW_OPENGL);
    HWND* hwnd;

    DWORD myThreadID;
    myHandle = CreateThread(0, 0, getMuseResult,  &progress, 0, &myThreadID);

    //Initialisation
    Mat img = imread("assets/init.png", IMREAD_COLOR);
    imshow("Initialisation", img);

    int key = waitKey();
    if (key == 32) {
        anaglyph = false;
    }
    else if (key == 13) {
        anaglyph = true;
    }
    else {
        return 0;
    }
    cvDestroyWindow("Initialisation");

    while (true) {
        Mat currentFrame = cameraStream->getCurrentFrame();
        Size textSize1 = getTextSize("Normal Mode", FONT_HERSHEY_PLAIN, 4, 1, 0);
        Size textSize2 = getTextSize("3D Mode", FONT_HERSHEY_PLAIN, 4, 1, 0);
        Size textSize3 = getTextSize("3", FONT_HERSHEY_PLAIN, 6, 1, 0);
        Size textSize4 = getTextSize("Inclinaison trop horizontale", FONT_HERSHEY_PLAIN, 2, 1, 0);
        Size textSize5 = getTextSize("Inclinaison trop verticale", FONT_HERSHEY_PLAIN, 2, 1, 0);
        if (!anaglyph) {
            putText(currentFrame, "Normal Mode", Point2i((currentFrame.cols - textSize1.width) / 2, currentFrame.rows - 30), FONT_HERSHEY_PLAIN, 4, Scalar(0, 0, 255), 2);
        }
        else if (anaglyph) {
            putText(currentFrame, "3D Mode", Point2i((currentFrame.cols - textSize2.width) / 2, currentFrame.rows - 30), FONT_HERSHEY_PLAIN, 4, Scalar(0, 0, 255), 2);
        }
        float long12, long03, long01, long23, ratio = 0;
        EdgeDetection ED = EdgeDetection(currentFrame, true);
        vector<Point2i> coordCorner = ED.getCorner(currentFrame);

        long12 = sqrt(pow(coordCorner[1].x - coordCorner[2].x, 2) + pow(coordCorner[1].y - coordCorner[2].y, 2));
        long03 = sqrt(pow(coordCorner[0].x - coordCorner[3].x, 2) + pow(coordCorner[0].y - coordCorner[3].y, 2));

        if (long03 > long12 * 0.9 && long03 < long12 * 1.1) {
            long01 = sqrt(pow(coordCorner[0].x - coordCorner[1].x, 2) + pow(coordCorner[0].y - coordCorner[1].y, 2));
            long23 = sqrt(pow(coordCorner[2].x - coordCorner[3].x, 2) + pow(coordCorner[2].y - coordCorner[3].y, 2));

            ratio = long01 / long23;
        }
        bool is45 = ratio > 0.73 && ratio < 0.8;

        if (ratio > 0.73 && ratio < 0.8) {

        }
        else if (ratio < 0.8) {
            putText(currentFrame, "Inclinaison trop horizontale", Point2i((currentFrame.cols - textSize4.width) / 2, currentFrame.rows - 100), FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 255), 2);
        }
        else if (ratio > 0.73) {
            putText(currentFrame, "Inclinaison trop verticale", Point2i((currentFrame.cols - textSize5.width) / 2, currentFrame.rows - 100), FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 255), 2);
        }

        if (is45 && timer == false) {
            //tick = getTickCount();
            bool timer = true;
        }
        else {
            tick = getTickCount();
            bool timer = false;
            putText(currentFrame, "Placez labyrinthe face a la camera", Point2i(0, 50), FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 255), 2);
        }
        int64 tick2 = getTickCount();
        int64 sec = (double)(tick2 - tick) / getTickFrequency();
        //cout << ratio << endl;

        if (sec == 1) {
            putText(currentFrame, "3", Point2i((currentFrame.cols - textSize3.width) / 2, 100), FONT_HERSHEY_PLAIN, 6, Scalar(0, 255, 0), 2);
        }
        if (sec == 2) {
            putText(currentFrame, "2", Point2i((currentFrame.cols - textSize3.width) / 2, 100), FONT_HERSHEY_PLAIN, 6, Scalar(0, 255, 0), 2);
        }
        if (sec == 3) {
            putText(currentFrame, "1", Point2i((currentFrame.cols - textSize3.width) / 2, 100), FONT_HERSHEY_PLAIN, 6, Scalar(0, 255, 0), 2);
        }
        if (sec == 4) {
            //Lancement du jeu
            Mat currentFrame = cameraStream->getCurrentFrame();
            double ratio = (double)currentFrame.cols / (double)currentFrame.rows;
            int width = 1000; /// Largeur de la fenêtre

            auto* glutMaster = new GlutMaster();
            window = new OpenGL(glutMaster, width, (int)(width / ratio), 0, 0, (char*)("aMAZEd"), ball, cameraStream, anaglyph, progress);

            setupMaze();
            window->startTimer();

            destroyWindow(name);
            glutMaster->CallGlutMainLoop();
        }


        hwnd = (HWND*)(cvGetWindowHandle(name.c_str()));
        if (hwnd == nullptr) {
            // CloseHandle(myHandle);
            delete cameraStream;
            delete window;
            delete angleModel;
            return 0;
        }
        imshow(name, currentFrame);

        int key = waitKey(20);

    }

    // CloseHandle(myHandle);
    delete cameraStream;
    delete window;
    delete angleModel;


    return 0;
}

//Fonction de calcul de distance entre deux points p1 et p2
double calculateDistance(cv::Point p1, cv::Point p2)
{
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

void loop(int endGame){

    window->setProgress(progress);

    if(endGame == 1){
        //CloseHandle(myHandle);
        waitKey(0);
        return;
    }

    /// Affichage FPS
    frame++;
    myTime = glutGet(GLUT_ELAPSED_TIME);
    if (myTime - timebase > 1000) {
        fps = frame * 1000.0 / (myTime - timebase);
        timebase = myTime;
        frame = 0;
    }
    window->setFps(fps);

    vector<Point2i> coordCorner;
    Mat currentFrame = cameraStream->getCurrentFrame();
    EdgeDetection edgeDetection = EdgeDetection(currentFrame, false);
    coordCorner = edgeDetection.getCorner(currentFrame);

    double offSetBall = 0.08;

    /// Si les 4 coins ont été détéctées
    if (coordCorner.size() == 4 && !edgeDetection.isReversed(coordCorner)) {

        double aire = calculateDistance(coordCorner[0], coordCorner[1]) * calculateDistance(coordCorner[1], coordCorner[2]);
        //cout << aire << endl;

        /// Calcul de l'aire pour réduire les saccades
        if (aire > 20000.0 && aire < 170000.0) {

            Transformation transformation = Transformation(coordCorner, Size(currentFrame.cols, currentFrame.rows), 0.1, 20);
            angleModel->setCurrentTransformation(&transformation);

            vector<Wall> walls;
            if (CollisionDetection::findCollisions(ball, window->getWalls(), walls)) {

                //Incrémentation du nombre de collisions
                window->incrementNBCollisions();

                /// Detection de la nature de la collision
                bool verticalCollision = false;
                bool horizontalCollision = false;
                for (auto& wall : walls) {
                    if (!verticalCollision && wall.isVertical()) verticalCollision = true;
                    if (!horizontalCollision && !wall.isVertical()) horizontalCollision = true;
                }

                /// Collision verticale on rebondit selon l'axe X
                if (verticalCollision) {
                    ball->setNextX(ball->getNextX() - ball->getVx());
                    //ball->setVx(-ball->getVx());
                    if (ball->getVx() >= 0) {
                        ball->setVx(-offSetBall);
                    }
                    else {
                        ball->setVx(offSetBall);
                    }
                    ball->setAx(0);
                }

                /// Collision horizontale on rebondit selon l'axe Y
                if (horizontalCollision) {
                    ball->setNextY(ball->getNextY() - ball->getVy());
                    //ball->setVy(-ball->getVy());

                    if (ball->getVy() >= 0) {
                        ball->setVy(-offSetBall);
                    }
                    else {
                        ball->setVy(offSetBall);
                    }
                    ball->setAy(0);
                }

                ball->updatePosition();

                /// S'il s'agit d'une collision sur le bout du mur
                /*if(CollisionDetection::findCollisions(ball, window->getWalls(), walls)){
                    if(verticalCollision){
                        ball->setNextY(ball->getNextY() - ball->getVy() * 2);
                        if(ball->getVy() > 0){
                            ball->setVy(-offSetBall);
                        }else{
                            ball->setVy(offSetBall);
                        }
                        ball->setAy(0);
                    }

                    if(horizontalCollision){
                        ball->setNextX(ball->getNextX() - ball->getVx() * 2);
                        if(ball->getVx() > 0){
                            ball->setVx(-offSetBall);
                        }else{
                            ball->setVx(offSetBall);
                        }
                        ball->setAx(0);
                    }
                }*/

                //cout << "R=" << ball->getR() << endl;


            }
            else {
                ball->setAx(angleModel->getAngleY() / 10);
                ball->setAy(angleModel->getAngleX() / 10);
                ball->updatePosition();
            }

            double p[16];
            double m[16];
            transformation.getProjectionMatrix(p);
            transformation.getModelviewMatrix(m);
            window->setProjectionMatrix(p);
            window->setModelviewMatrix(m);
        }
    }

    glutPostRedisplay();

}

void setupMaze(){

    /// Détection des coins
    Mat currentFrame = cameraStream->getCurrentFrame();
    EdgeDetection edgeDetection = EdgeDetection(currentFrame, true);

    vector<Point2i> coordCorner;
    vector<Point2i> coordStartEnd;
    vector<vector<Point2i>> lines;

    /// Tant que les 4 coins n'ont pas été détéctées
    do {

        currentFrame = cameraStream->getCurrentFrame();
        coordStartEnd = edgeDetection.startEndDetection(currentFrame);
        coordCorner = edgeDetection.getCorner(currentFrame);

        /// Detection des murs
        lines = edgeDetection.wallsDetection(currentFrame, coordCorner, coordStartEnd);

    }while(coordStartEnd.size() != 2);

    Transformation *transformation = new Transformation(coordCorner, Size(currentFrame.cols, currentFrame.rows), 1, 10);

    ///point d'arrivée sauvegarde
    Point2d *pointModelEnd = new Point2d(transformation->getModelPointFromImagePoint(coordStartEnd[1]));
    window->setEndPoint(pointModelEnd);

    ///set la boule aux coordonnées du départ détectés
    cv::Point2d pointModelStart = transformation->getModelPointFromImagePoint(coordStartEnd[0]);

    ///set la boule aux coordonnées du départ
    ball->setNextX(pointModelStart.x);
    ball->setNextY(pointModelStart.y);

    /// Calcul des coordonées des extrimités des murs
    vector<Wall> walls;
    for (const auto &line : lines) {

        Point2d pointImageA = transformation->getModelPointFromImagePoint(line[0]);
        Point2d pointImageB = transformation->getModelPointFromImagePoint(line[1]);

        Wall wall(pointImageA, pointImageB);

        walls.push_back(wall);
    }

    /// Murs extérieurs
//    walls.emplace_back(Point2d(0, 0), Point2d(0, 1));
//    walls.emplace_back(Point2d(1, 1), Point2d(0, 1));
//    walls.emplace_back(Point2d(1, 1), Point2d(1, 0));
//    walls.emplace_back(Point2d(1, 0), Point2d(0, 0));

    window->setWalls(walls);

    angleModel = new AngleModel(transformation);


}


DWORD WINAPI getMuseResult(LPVOID lpProgress) {

    unsigned int& progress = *((unsigned int*)lpProgress);

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0){
        cout << "WSAStartup failed.\n";
        system("pause");
        return 1;
    }

    // Create a server hint structure for the server
    sockaddr_in serverHint;
    serverHint.sin_addr.S_un.S_addr = ADDR_ANY; // Us any IP address available on the machine
    serverHint.sin_family = AF_INET; // Address format is IPv4
    serverHint.sin_port = htons(9436); // Convert from little to big endian

    SOCKET in = socket(AF_INET, SOCK_DGRAM, 0);

    // Try and bind the socket to the IP and port
    int iResult = ::bind(in, (sockaddr*)&serverHint, sizeof(serverHint));
    if (iResult == SOCKET_ERROR)
    {
        cout << "Can't bind socket! " << WSAGetLastError() << endl;
        return 1;
    }

    ////////////////////////////////////////////////////////////
    // MAIN LOOP SETUP AND ENTRY
    ////////////////////////////////////////////////////////////

    sockaddr_in client; // Use to hold the client information (port / ip address)
    int clientLength = sizeof(client); // The size of the client information

    char buffer[1024];

    while(true){
        
        ZeroMemory(&client, clientLength); // Clear the client structure
        ZeroMemory(buffer, 1024); // Clear the receive buffer

        // Wait for message
        int bytesIn = recvfrom(in, buffer, 1024, 0, (sockaddr*)&client, &clientLength);
        if (bytesIn == SOCKET_ERROR)
        {
            cout << "Error receiving from client " << WSAGetLastError() << endl;
            continue;
        }

        // Display message and client info
        char clientIp[256]; // Create enough space to convert the address byte array
        ZeroMemory(clientIp, 256); // to string of characters

        // Convert from byte array to chars
        inet_ntop(AF_INET, &client.sin_addr, clientIp, 256);

        // Display the message / who sent it
        cout << buffer << endl;

        int start = 0;
        while (buffer[start] != '=') start++;
        int middle = start + 1;
        while (buffer[middle] != '/') middle++;
        int end = start + 1;
        while (buffer[end] != ';') end++;

        //Isolation des données de stress
        char firstProgress = buffer[start];
        char lastProgress = buffer[start+1];

        string progress_str;
        for (int i = start+1; i < middle; i++) {
            progress_str.push_back(buffer[i]);
        }

        progress = (unsigned int)atoi(progress_str.c_str());

        //Isolation des données de date
        string time_str;
        for (int i = start + 2; i < end; i++) {
            time_str.push_back(buffer[i]);
        }

        __int64 time = _atoi64(time_str.c_str());
        __int64 now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        //différence de temps entre la réception du signal sur l'android et la réception de la donnée sur le labyrinthe
        if(diff < 0){
            diff = now - time;
        } else {
            diff = (diff + now - time)/2;
        }
        //cout << "Diff:" << diff << endl;


    }
}
