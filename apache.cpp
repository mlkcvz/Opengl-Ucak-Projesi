

#include <array>    //std::array kullanmak için gerekiyor. draw_txt fonksiyonu içinde puan tablosunu yazarken karakterler array içine atiliyor.
#include <charconv> //std::to_chars kullanmak için gerekiyor. draw_txt fonksiyonu içinde puan tablosunu yazarken kullanılıyor.
#include <time.h>   //Rastgele sayı üretirken o anki zamanı çekmek için.
#include <math.h>   // Sin ve cos fonksiyonları için
#include <vector>   //vectorleri kullanmak icin
#include <glut.h>

int plane_X = 3000, plane_Y = 6500, life = 3, score = 0, speed = 2000, fps;
float timepass = 0, fpstimer = 0, apache_speed = 0, xrot = 180, moving_color = 0;

bool breath = true;

class apache;
std::vector<apache> attack_helicopters;

/*
    Tuş basmaları windowsta tam algılanmıyordu. Bunu 
    asmak icin inputlar geldiğinde direk ucağın yerini değiştirmek yerine
    bu integerin bit değerleri kullanılıyor. Her bit bir yönü temsil ediyor.

    0001 YUKARI
    0010 ALT
    0100 SAĞ
    1000 SOL

    Bu şekilde hem haraket daha seri oluyor hem de bu sistem sayesinde 
    hem yukarı hem sola veya sağa gidebiliyorsunuz.
    Örn:
    1001 değerini alırsa hem YUKARI hen SOLA gidiyor.

*/
struct input
{
    unsigned int key_values : 4;
};

input input_logic;

/*
    Zoom fonskiyonu

    Zoom yapılması istenen yerden önce cağrılıp
    sonunda clear() kullanılmalı.
*/
void zoom_in(int scale, int x, int y)
{
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(scale, scale, 0);
    glTranslatef(-x, -y, 0);
}

/*
    Rotate fonskiyonu

    Çevri yapılması istenen yerden önce cağrılıp
    sonunda clear() kullanılmalı.
*/
void turn(float angle, int x, int y)
{
    glPushMatrix();
    glTranslatef(x, y, 0);
    glRotatef(angle, 0, 1, 0);
    glTranslatef(-x, -y, 0);
}

void clear()
{
    glPopMatrix();
}

/*    
    Verilen bir dairenin çizilmesi
    cx ve cy merkez noktanın kordinatları
    R yarıçapı    
    Num_segmentte ne kadar detaylı çizileceğini belirtiyor.

    Glut üzerinde daire çizme fonksiyonu yok aslında yine poligonlar çiziliyor. 
    örneğin num_segments 100 ise 100 kenarlı bir poligon çiziyoruz. Ne kadar detaylı çizilirse
    ekranda o kadar daire ilizyonu oluşuyor.
*/
void DrawCircle(float cx, float cy, float r, int num_segments)
{
    glBegin(GL_POLYGON);
    for (int i = 0; i < num_segments; i++)
    {
        float theta = 2.0f * 3.14f * float(i) / float(num_segments); //Kenarların açıları

        float x = r * cosf(theta); //x in verilen açı kadar
        float y = r * sinf(theta); //calculate the y component

        glVertex2f(x + cx, y + cy); //output vertex
    }
    glEnd();
}

struct COLOR
{
    GLfloat r = rand() / (RAND_MAX / 0.9), g = rand() / (RAND_MAX / 0.9), b = rand() / (RAND_MAX / 0.9);
    float a = 1.0f;
};

class apache
{
public:
    int x = 0, y = 0;
    float speed = 1000 + apache_speed;
    int xrot = 0;
    COLOR color;
    bool left_to_right = false; //
    void draw()
    {

        /*        
        soldan sağa gidenleri ters döndüren kod bloğu
        Bu şekilde gittikleri yöne bakıyorlar.
        */
        if (left_to_right)
        {
            turn(180, x, y);
        }

        /*
            Orta tarafı bir çok parçadan çiziliyor. 
            Rakamlar örnek bir resmin pixel noktalarına göre o resimden alındı.
            Resin boyutu mantıksal kordinat sistemde istenen yüksekliğe ayarlandı.
            Bu şekilde istenen noktalardan yaklaşık olarak olması gereken nokta değeri
            fare ile bakılarak alındı.
        */

        glColor4f(color.r, color.g, color.b, 0.7);
        glBegin(GL_POLYGON);

        glVertex2i(x, y + 550);
        glVertex2i(x + 5, y + 516);
        glVertex2i(x + 87, y + 465);
        glVertex2i(x + 1200, y + 280);
        glVertex2i(x + 1213, y + 322);
        glVertex2i(x + 841, y + 406);
        glEnd();

        glBegin(GL_POLYGON);

        glVertex2i(x + 94, y + 466);
        glVertex2i(x + 110, y + 434);
        glVertex2i(x + 204, y + 364);
        glVertex2i(x + 661, y + 292);
        glVertex2i(x + 767, y + 360);
        glEnd();

        glBegin(GL_POLYGON);

        glVertex2i(x + 282, y + 340);
        glVertex2i(x + 286, y + 326);
        glVertex2i(x + 385, y + 309);
        glVertex2i(x + 423, y + 298);
        glVertex2i(x + 590, y + 296);
        glVertex2i(x + 662, y + 293);
        glVertex2i(x + 280, y + 370);
        glEnd();

        glBegin(GL_POLYGON);
        glVertex2i(x + 370, y + 270);
        glVertex2i(x + 412, y + 260);
        glVertex2i(x + 423, y + 302);
        glVertex2i(x + 385, y + 311);
        glEnd();

        glBegin(GL_POLYGON);
        glVertex2i(x + 1200, y + 270);
        glVertex2i(x + 1270, y + 34);
        glVertex2i(x + 1180, y + 46);
        glVertex2i(x + 1100, y + 286);

        glBegin(GL_POLYGON);
        glVertex2i(x + 840, y + 412);
        glVertex2i(x + 766, y + 446);
        glVertex2i(x + 34, y + 562);
        glVertex2i(x + 5, y + 550);
        glEnd();
        glColor4f(color.r, color.g, color.b, color.a);
        /*
            Tekerler
            Ön teker 1
            Ön teker 2
            Arka teker
        */

        glBegin(GL_LINES);
        glColor4f(0, 0, 0, 0.5);
        glVertex2i(x + 255, y + 527);
        glVertex2i(x + 325, y + 592);
        glVertex2i(x + 271, y + 521);
        glVertex2i(x + 370, y + 580);
        glEnd();

        glBegin(GL_LINES);
        glColor4f(0, 0, 0, 0.5);
        glVertex2i(x + 255, y + 527);
        glVertex2i(x + 325, y + 592);
        glVertex2i(x + 271, y + 521);
        glVertex2i(x + 370, y + 580);
        glEnd();

        glLineWidth(1.5);
        glBegin(GL_LINES);
        glVertex2i(x + 1200, y + 321);
        glVertex2i(x + 1264, y + 351);
        glEnd();

        DrawCircle(x + 371, y + 582, 25, 200);
        DrawCircle(x + 323, y + 590, 25, 200);
        DrawCircle(x + 1273, y + 360, 25, 100);

        /*
            Camlar
            Ön cam
            Arka Cam
        */
        glBegin(GL_POLYGON);
        glColor4f(0.729, 0.709, 0.709, 1);
        glVertex2i(x + 98, y + 493);
        glVertex2i(x + 95, y + 470);
        glVertex2i(x + 113, y + 438);
        glVertex2i(x + 192, y + 383);
        glVertex2i(x + 202, y + 430);
        glVertex2i(x + 125, y + 483);
        glEnd();

        glBegin(GL_POLYGON);
        glColor4f(0.729, 0.709, 0.709, 1);
        glVertex2i(x + 217, y + 438);
        glVertex2i(x + 205, y + 376);
        glVertex2i(x + 293, y + 350);
        glVertex2i(x + 294, y + 361);
        glVertex2i(x + 269, y + 419);
        glEnd();

        /*
        Pervaneler
        Arka  
        Ön     
        */
        glBegin(GL_TRIANGLES);
        glColor4f(0, 0, 0, 1);
        //Merkez glVertex2i(x + 1200, y + 100);
        glVertex2i(x + 1200, y + 100);
        glVertex2i(x + 1200 + sin(moving_color * 3.14) * 200, y + 100 + cos(moving_color * 3.14) * 200);
        glVertex2i(x + 1200 + sin((moving_color + 360.0f) * 3.14) * 200, y + 100 + cos((moving_color + 360.0f) * 3.14) * 200);

        glColor4f(1, 0, 0, 1);
        glVertex2i(x + 1200, y + 100);
        glVertex2i(x + 1200 + sin((xrot)*3.14 / 180.0f) * 200, y + 100 + cos((xrot)*3.14 / 180.0f) * 200);
        glVertex2i(x + 1200 + sin((xrot + 22) * 3.14 / 180.0f) * 200, y + 100 + cos((xrot + 33) * 3.14 / 180.0f) * 200);

        glVertex2i(x + 1200, y + 100);
        glVertex2i(x + 1200 + sin((xrot + 90) * 3.14 / 180.0f) * 200, y + 100 + cos((xrot + 90) * 3.14 / 180.0f) * 200);
        glVertex2i(x + 1200 + sin((xrot + 90 + 33) * 3.14 / 180.0f) * 200, y + 100 + cos((xrot + 90 + 33) * 3.14 / 180.0f) * 200);

        glVertex2i(x + 1200, y + 100);
        glVertex2i(x + 1200 + sin((xrot + 180) * 3.14 / 180.0f) * 200, y + 100 + cos((xrot + 180) * 3.14 / 180.0f) * 200);
        glVertex2i(x + 1200 + sin((xrot + 180 + 33) * 3.14 / 180.0f) * 200, y + 100 + cos((xrot + 180 + 33) * 3.14 / 180.0f) * 200);

        glVertex2i(x + 1200, y + 100);
        glVertex2i(x + 1200 + sin((xrot + 270) * 3.14 / 180.0f) * 200, y + 100 + cos((xrot + 270) * 3.14 / 180.0f) * 200);
        glVertex2i(x + 1200 + sin((xrot + 270 + 33) * 3.14 / 180.0f) * 200, y + 100 + cos((xrot + 270 + 33) * 3.14 / 180.0f) * 200);
        glEnd();

        glBegin(GL_TRIANGLE_FAN);
        glColor4f(0, 0, 0, 1);
        glVertex2i(x + 380, y + 240);
        glVertex2i(x + 380 * 2 + sin(xrot) * 200, y + 160 - cos(xrot) * 25);
        glVertex2i(x - 180 + sin(xrot * 3.14) * 200, y + 320 + cos(xrot * 3.14) * 25);
        glEnd();

        //Silahlar
        glBegin(GL_LINES);
        glVertex2i(x + 25, y + 535);
        glVertex2i(x - 50, y + 555);

        glVertex2i(x + 35, y + 545);
        glVertex2i(x - 40, y + 565);
        glEnd();

        clear();
    }

    /*
        Çarpışma testi

        Şekiller için dairesel merkezler belirledik. 
        O dairelerin anlık olarak merkezlerine olan uzaklıkları toplamı
        Eğer kendi yarı çapların toplamından büyükse o iki daire
        iç içe geçmiş olmak zorunda. Yani çarpışmış olmaları gerekiyor.

        Ucak için 1 daire 
        Helikopterler için 2 daire üzerinden hesap yapılıyor.
        
    */
    bool crash_test()
    {
        int plane_r = 280;       //Ucak dairesi yarı çap
        int cpx = plane_X + 295; //Ucak çiziminin merkezi x kordinatı
        int cpy = plane_Y + 295; //Ucak çiziminin merkezi y kordinatı

        int my_r = 200;            // helicopter dairesi yarı çap
        int my_center_x = x + 250; // helicopter için ilk dairenin merkezi
        int my_center_y = y + 400; // helicopter için ilk dairenin merkezi

        if (left_to_right)
        {
            my_center_x = x - 250; // Helicopterler rotate işlemi izle çizildiyse merkez x kordinatları tam tersi yöne olacak
        }

        int distance_x = cpx - my_center_x; //x uzaklığı
        int distance_y = cpy - my_center_y; //y uzaklığı
        int radius_sum = plane_r + my_r;    //yarı çap toplamları

        /*
            Yarı çap toplamlarının uzaklıklar toplamına kıyaslanması
        */
        if (distance_x * distance_x + distance_y * distance_y <= radius_sum * radius_sum)
        {
            return true;
        }

        my_center_x = x + 840; // helicopter için ikinci dairenin merkezi
        my_center_y = y + 220; // helicopter için ikinci dairenin merkezi

        if (left_to_right)
        {
            my_center_x = x - 840; // Helicopterler rotate işlemi izle çizildiyse merkez x kordinatları tam tersi yöne olacak
        }

        distance_x = cpx - my_center_x; //Uzaklık hesaplanması
        distance_y = cpy - my_center_y; //Uzaklık hesaplanması

        /*
            Yarı çap toplamlarının  helikopter için ikinci  daire olan uzaklıklar toplamına kıyaslanması
        */

        if (distance_x * distance_x + distance_y * distance_y <= radius_sum * radius_sum)
        {
            return true;
        }

        /*
            Kod buraya geldiyse çarpışma olmamış demektir. O zaman false değeri veriliyor.
        */
        return false;
    }
};


void draw_plane()
{
    glBegin(GL_POLYGON);
    glColor4f(1, 0.670, 0.670, 1);
    glVertex2i(plane_X + 300, plane_Y);
    glColor4f(0, 0.670, 0.670, 1);
    glVertex2i(plane_X + 325, plane_Y + 40);
    glColor4f(0.5, 0.670, 0.670, 1);
    glVertex2i(plane_X + 330, plane_Y + 59);
    glColor4f(0.5, 1, 0.670, 1);
    glVertex2i(plane_X + 340, plane_Y + 75);
    glVertex2i(plane_X + 340, plane_Y + 380);
    glVertex2i(plane_X + 250, plane_Y + 380);

    glEnd();

    glBegin(GL_POLYGON);
    glColor4f(0.5, 0.670, 0.670, 1);
    glVertex2i(plane_X + 340, plane_Y + 240);
    glVertex2i(plane_X + 550, plane_Y + 350);
    glVertex2i(plane_X + 550, plane_Y + 400);
    glVertex2i(plane_X + 340, plane_Y + 330);
    glEnd();

    glBegin(GL_POLYGON);
    glColor4f(0.5, 1, 0.670, 1);
    glVertex2i(plane_X + 320, plane_Y + 360);
    glVertex2i(plane_X + 320, plane_Y + 560);
    glVertex2i(plane_X + 260, plane_Y + 560);
    glVertex2i(plane_X + 260, plane_Y + 360);
    glEnd();

    glBegin(GL_POLYGON);
    glColor4f(0.5, 1, 0.670, 1);
    glVertex2i(plane_X + 285, plane_Y + 560);
    glVertex2i(plane_X + 300, plane_Y + 560);
    glVertex2i(plane_X + 300, plane_Y + 620);
    glVertex2i(plane_X + 285, plane_Y + 620);
    glEnd();

    glBegin(GL_POLYGON);
    glColor4f(0.5, 1, 0.670, 1);
    glVertex2i(plane_X + 320, plane_Y + 500);
    glVertex2i(plane_X + 400, plane_Y + 560);
    glVertex2i(plane_X + 400, plane_Y + 600);
    glVertex2i(plane_X + 320, plane_Y + 560);
    glEnd();
}

/*
    Üst tarafın çizildiği ve yazıldığı yer. Küçük yazılar için glutBitmapCharacter kullanıldı fakat 
    glutBitmapCharacter scale ve benzeri komutlardan etkilenmiyor bu yüzden  yazıları daha büyük 
    göstermek istenen yerlerde  "glutStrokeCharacter" ve scale komutları kullanıldı. 
*/
void draw_txt()
{
    glLineWidth(1);

    glBegin(GL_POLYGON);
    glColor4f(0, 0, 0, 0.7);
    glVertex2i(0, 0);
    glVertex2i(6000, 0);
    glVertex2i(6000, 400);
    glVertex2i(0, 400);
    glEnd();

    glColor4f(1, 1, 1, 1);

    /*
        Zamanın gösterildiği kod bloğu. 
    */
    glRasterPos2i(100, 230);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'S');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'u');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'r');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'e');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ':');
    std::array<char, 5> timechar = {};
    std::to_chars(timechar.data(), timechar.data() + timechar.size(), (int)(timepass / 1000));
    for (int i = 0; i < timechar.size(); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, timechar[i]);
    }

    /*
        Fps gösterilen kod bloğu
    */
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'F');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'P');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'S');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, ':');
    std::array<char, 4> fpschar = {};
    std::to_chars(fpschar.data(), fpschar.data() + fpschar.size(), (int)(fps / (fpstimer)));
    for (int i = 0; i < fpschar.size(); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, fpschar[i]);
    }

    glPushMatrix();
    glScalef(3, 2.5, 0);     //İstenilen derecede büyütme yapılıyor.
    glRotatef(180, 0, 0, 1); //Yazının yeri ve açısı ayarlanıyor.
    glTranslatef(-1000, -130, 0);
    glRotatef(180, 0, 1, 0);

    char ar[20] = "Puan:0 Hak:3";
    for (int i = 0; i < 20; i++)
    {
        glColor4f(1, i * (moving_color / 1000.0f), 1, 1); //Kayan morumsu  renk
        if (ar[i] == '0')
        {
            glColor4f(0, i * (moving_color / 1000.0f), 1, 1);
            std::array<char, 3> str = {'0', '0', '0'};
            if (score < 100)
            {
                std::to_chars(str.data() + 1, str.data() + str.size(), score);
            }
            else
            {
                std::to_chars(str.data(), str.data() + str.size(), score);
            }

            for (int k = 0; k < str.size(); k++)
            {
                glutStrokeCharacter(GLUT_STROKE_ROMAN, str[k]);
            }
        }
        else if (ar[i] == '3')
        {
            glColor4f(0.5, i * (moving_color / 1000.0f), 1, 1);
            std::array<char, 1> str;
            std::to_chars(str.data(), str.data() + str.size(), life);
            glutStrokeCharacter(GLUT_STROKE_ROMAN, str[0]);
        }
        else
        {
            glutStrokeCharacter(GLUT_STROKE_ROMAN, ar[i]);
        }
    }

    glPopMatrix(); //Matrix tekrar eski ayarlara geçiyor.
}

/*
    Çizim fonksiyonu
    Uçak çiziliyor.
    Ust taraf çiziliyor.
    Helicopterler çiziliyor.
*/
void show()
{

    glClear(GL_COLOR_BUFFER_BIT);

    glColor4f(0.976, 0.968, 0.905, 1);
    glBegin(GL_POLYGON);

    glVertex2i(0, 0);
    glVertex2i(0, 8000);
    glVertex2i(6000, 8000);
    glVertex2i(6000, 0);
    glEnd();

    /*
        Uçağın bir tarafı çiziliyor.
        Sonra diğer tarafı aynı çizimin aynalanması şeklide çiziliyor.
    */
    
    draw_plane();    
    turn(180, plane_X + 300, plane_Y);
    draw_plane();
    clear();

    draw_txt();

    for (auto apaches : attack_helicopters)
    {
        apaches.draw();
    }
   
    glutSwapBuffers();
}
void logic()
{

    /*
        Zaman değerlerinin alındığı blok
        Aradaki fark alınarak işlemci hızından bağımsız olarak
        haraket hizini dt'yi kullanarak belli bir zamanda 
        sabit bir birim ilerlenmesi sağlanıyor.
    */
    int t;
    float dt;
    t = glutGet(GLUT_ELAPSED_TIME);
    dt = (t - timepass) / 1000.0;
    timepass = t;
    fpstimer += dt;
    fps++;
    if (fpstimer > 1)
    {
        fps = 1;
        fpstimer = dt;
    }


    /*
        Can sifirin altına düştüyse, oyun bitmiş demektir. 
        Oyun bittiyse hesaplama yapmıyoruz.
    */
    if (life < 1)
    {
        glutPostRedisplay();
        return;
    }

    /*
        Uçak hareketleri
        input_logic içindeki 4 bitlik değere göre gitmesi gereken yöne
        doğru gidiyor. Örnek olarak key_value 
        0001 değerinde ise if YUKARI bloğuna giriyor.
        0001&0001=true
    */
    if (input_logic.key_values & 1)
    { //YUKARI
        plane_Y -= 1 * speed * dt;

        /*
            Karşı tarafa ulaşınca puan artışı ve
            zorluğun arttırılması.
        */
        if (plane_Y < 0)
        {
            score += 10;
            apache_speed+=500;
            plane_X = 3000 - 625 / 2;
            plane_Y = 6500 - 625 / 2;
        }
    }

    /*
        Verilen değerler ekran limitini aşmıyorsa basılan tuşa göre
        istenen yöne  hız*dt kadar iteleme yapılıyor.
        Eğer kenarlarda isek plane_X ve Y sabit kalıyor.
    */
    if (input_logic.key_values & 2)
    {                                                                             //AŞAĞI
        plane_Y = (plane_Y < 8000 - 625) ? plane_Y + 1 * speed * dt : 8000 - 625; //Alt kenar limiti
    }
    if (input_logic.key_values & 4)
    {                                                                             //SAĞA
        plane_X = (plane_X < 6000 - 625) ? plane_X + 1 * speed * dt : 6000 - 625; //Sağ kenar limiti.
    }
    if (input_logic.key_values & 8)
    {                                                           //SOLA
        plane_X = (plane_X > 0) ? plane_X - 1 * speed * dt : 0; //Sol kenar limiti
    }


    /*
        Helicopterlerin işlenmesi
        Çarpışma testide burada kontrol ediliyor.
    */
    for (auto &apaches : attack_helicopters)
    {
        apaches.speed=1000+apache_speed;//Oyun seviyesine göre daha hızlı gidiyorlar.
        if (apaches.left_to_right)
        {
            apaches.x = (apaches.x > 6000 + 1350) ? -625 : apaches.x + apaches.speed * dt;
        }
        else
        {
            apaches.x = (apaches.x < 0 - 1350) ? 6000 + 625 : apaches.x - apaches.speed * dt;
        }
        apaches.xrot = (apaches.xrot + 1 * 5) % 360;//Pervanelerin dönüş hızı

        if (apaches.crash_test())
        {
            life--;
            plane_X = 3000 - 625 / 2;
            plane_Y = 6500- 625 / 2;
        }
    }

        /*
    Yazı renkleri  için belli bir aralıkta artan ve azalan bir değerlerin ayarlandığı blok.
    */
    moving_color = (breath) ? (moving_color + 50 * dt) : (moving_color - 50 * dt); //Canlı renkli yazı için nefes almayı taklit eden değişken.
    if (moving_color > 360)
    {
        breath = false;
    }
    if (moving_color < 1)
    {
        breath = true;
    }


    glutPostRedisplay();
}

/*
Pencere genişletilince görüntünün esnemesi engellemek için viewportu sabit  ve ortada tutan kod bloğu.
*/
void donothing(int x, int y)
{
    glViewport(x / 2 - 240, y / 2 - 320, 480, 2 * 320);
}

/*
    Tuşlara basıldığı zaman 
*/
void arrow_keys_press(int arrow, int x, int y)
{

    if (arrow == GLUT_KEY_UP)
    {
        input_logic.key_values |= 1;
    }

    if (arrow == GLUT_KEY_DOWN)
    {
        input_logic.key_values |= 2;
    }

    if (arrow == GLUT_KEY_RIGHT)
    {
        input_logic.key_values |= 4;
    }

    if (arrow == GLUT_KEY_LEFT)
    {
        input_logic.key_values |= 8;
    }
}

/*
    Tuşlar bırakıldığı zaman
*/
void arrow_keys_release(int arrow, int x, int y)
{
    if (arrow == GLUT_KEY_UP)
    {
        input_logic.key_values &= 15 - 1;
    }

    if (arrow == GLUT_KEY_DOWN)
    {
        input_logic.key_values &= 15 - 2;
    }
 
    if (arrow == GLUT_KEY_RIGHT)
    {
        input_logic.key_values &= 15 - 4;
    }

    if (arrow == GLUT_KEY_LEFT)
    {
        input_logic.key_values &= 15 - 8;
    }
}
/*
    Enter tuşu
    Can 0 iken enter tuşuna basılırsa oyun baştan başlıyor
*/
void enter_key(unsigned char c,int x,int y){

    if (c==13 && life<1){
    plane_X = 3000 - 625 / 2;
    plane_Y = 6500 - 625 / 2;
     life = 3;
    score = 0;
    apache_speed=0;
    }
    
}

int main(int argc, char **arg)
{

    glutInit(&argc, arg);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(480, 640);
    glutIdleFunc(logic);
    glutCreateWindow("Title");
    glutDisplayFunc(show);
    glClearColor(1, 1, 1, 1);

    /*
        //Daha ince ayarlar ve çizimler yapabilmek için mantıksal çözünürlüğü 12.5 kat genişlettik. 
        Ekranda 50 pixel bir uzunluk mantıksal çözülürlükte 625 pixele denk geliyor.
    */
    
    gluOrtho2D(0, 6000, 8000, 0);

    glutReshapeFunc(donothing);
    srand((unsigned)time(0));
   
    timepass = glutGet(GLUT_ELAPSED_TIME);
    glutSpecialFunc(arrow_keys_press);
    glutSpecialUpFunc(arrow_keys_release);
    glutKeyboardFunc(enter_key);
    
    for (int i = 0; i < 4; i++)
    {
        apache temp;
        temp.x = rand()%6000;
        temp.y = i * 1000 + 625;
        temp.xrot = rand() % 360;
        if (i % 2 == 0)
            temp.left_to_right = true;
        attack_helicopters.emplace_back(temp);
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glutMainLoop();
    return 0;
}
