#include <Adafruit_NeoPixel.h>

#define LED_PIN 5
#define LED_COUNT 100

#define BTN_BLUE 14
#define BTN_GREEN 27
#define BTN_YELLOW 26

#define BUZZER_PIN 25

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// ---------------- STATE ----------------
int bossPos = LED_COUNT - 3;
int bossHP = 3;
bool bossAlive = true;

unsigned long lastMove = 0;
int enemyDelay = 300; // ms

String gamePhase = "hiatus";

struct Bullet
{
    int pos;
    uint32_t color;
};

Bullet bullets[20];
int bulletCount = 0;

struct Minion
{
    int pos;
    uint32_t color;
};

Minion minions[30];
int minionCount = 0;
int spawnIndex = 0;

int waveIndex = 0;
int waveSizes[3] = {10, 15, 20};

// ---------------- SOUND ----------------
void toneSimple(int freq, int dur)
{
    ledcAttachPin(BUZZER_PIN, 0);
    ledcWriteTone(0, freq);
    delay(dur);
    ledcWriteTone(0, 0);
}

void pew() { toneSimple(900, 80); }
void puf() { toneSimple(200, 50); }
void mehhh() { toneSimple(300, 180); }

void bossIntro()
{
    int base = 293 + (3 - bossHP) * 40;
    toneSimple(base, 80);
    toneSimple(base, 80);
    toneSimple(base, 200);
    toneSimple(base - 30, 150);
    toneSimple(base, 400);
}

void victory()
{
    toneSimple(523, 80);
    toneSimple(659, 80);
    toneSimple(783, 80);
    toneSimple(1046, 150);
    toneSimple(1567, 300);
}

// ---------------- RAINBOW ----------------
void rainbow()
{
    static int offset = 0;

    for (int i = 0; i < LED_COUNT; i++)
    {
        int hue = (i * 256 / LED_COUNT + offset) & 255;
        strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(hue * 256)));
    }
    strip.show();
    offset += 2;
    delay(20);
}

// ---------------- SPAWN ----------------
void spawnWave()
{
    minionCount = 0;
    spawnIndex = 0;
    gamePhase = "spawning";
}

void addMinion(int pos)
{
    minions[minionCount].pos = pos;

    int r = random(3);
    if (r == 0)
        minions[minionCount].color = strip.Color(0, 255, 0); // Verde
    if (r == 1)
        minions[minionCount].color = strip.Color(0, 0, 255); // Azul
    if (r == 2)
        minions[minionCount].color = strip.Color(255, 255, 0); // Amarillo

    minionCount++;
}

// ---------------- DRAW ----------------
void drawGame()
{
    strip.clear();

    if (bossAlive)
    {
        for (int i = 0; i < bossHP; i++)
            strip.setPixelColor(bossPos + i, strip.Color(160, 0, 200));
    }

    for (int i = 0; i < minionCount; i++)
        strip.setPixelColor(minions[i].pos, minions[i].color);

    for (int i = 0; i < bulletCount; i++)
        strip.setPixelColor(bullets[i].pos, bullets[i].color);

    strip.show();
}

// ---------------- SETUP ----------------
void setup()
{
    strip.begin();
    strip.show();

    pinMode(BTN_BLUE, INPUT_PULLUP);
    pinMode(BTN_GREEN, INPUT_PULLUP);
    pinMode(BTN_YELLOW, INPUT_PULLUP);
}

// ---------------- LOOP ----------------
void loop()
{

    if (gamePhase == "hiatus")
    {
        rainbow();

        if (!digitalRead(BTN_BLUE) || !digitalRead(BTN_GREEN) || !digitalRead(BTN_YELLOW))
        {
            bossAlive = true;
            bossHP = 3;
            bossPos = LED_COUNT - 3;
            waveIndex = 0;
            enemyDelay = 300;
            spawnWave();
        }
        return;
    }

    if (gamePhase == "spawning")
    {
        int currentSize = waveSizes[min(waveIndex, 2)];

        if (spawnIndex < currentSize)
        {
            addMinion(bossPos - 1 - spawnIndex);
            spawnIndex++;
            delay(80);
        }
        else
        {
            gamePhase = "playing";
            bossIntro();
            lastMove = millis();
        }
    }

    if (gamePhase == "playing")
    {
        if (!digitalRead(BTN_BLUE))
        {
            bullets[bulletCount++] = {0, strip.Color(0, 0, 255)};
            pew();
        }
        if (!digitalRead(BTN_GREEN))
        {
            bullets[bulletCount++] = {0, strip.Color(0, 255, 0)};
            pew();
        }
        if (!digitalRead(BTN_YELLOW))
        {
            bullets[bulletCount++] = {0, strip.Color(255, 255, 0)};
            pew();
        }
    }

    if (gamePhase == "playing" && millis() - lastMove > enemyDelay)
    {
        bossPos--;
        for (int i = 0; i < minionCount; i++)
            minions[i].pos--;
        lastMove = millis();
    }

    for (int i = 0; i < bulletCount; i++)
    {
        bullets[i].pos++;

        if (bullets[i].pos >= LED_COUNT)
        {
            mehhh();
            bossPos -= 2;
            for (int j = 0; j < minionCount; j++)
                minions[j].pos -= 2;

            bullets[i] = bullets[--bulletCount];
            i--;
            continue;
        }

        for (int j = 0; j < minionCount; j++)
        {
            if (bullets[i].pos == minions[j].pos)
            {
                if (bullets[i].color == minions[j].color)
                {
                    puf();
                    minions[j] = minions[--minionCount];
                }
                else
                {
                    mehhh();
                    bossPos -= 2;
                }
                bullets[i] = bullets[--bulletCount];
                i--;
                break;
            }
        }
    }

    if (gamePhase == "playing" && minionCount == 0)
    {
        bossHP--;
        waveIndex++;
        enemyDelay = max(80, enemyDelay - 40);

        if (bossHP > 0)
        {
            spawnWave();
        }
        else
        {
            victory();
            gamePhase = "hiatus";
        }
    }

    drawGame();
}