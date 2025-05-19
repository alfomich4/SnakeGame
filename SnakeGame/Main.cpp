#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <deque>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <string>

// --- Константы ---
static constexpr int WINDOW_WIDTH = 800;
static constexpr int WINDOW_HEIGHT = 600;
static constexpr int CELL_SIZE = 32;
static constexpr int GRID_WIDTH = WINDOW_WIDTH / CELL_SIZE;   // 25
static constexpr int GRID_HEIGHT = WINDOW_HEIGHT / CELL_SIZE; // 18
static const    std::string RES = "resources/";
static constexpr int MAX_POPUP_ENTRIES = 5;
static constexpr int MAX_MENU_ENTRIES = 10;

// --- Состояния игры ---
enum class GameState { Menu, Difficulty, Settings, Highscores, Playing, Paused, Death };

// --- Направления ---
enum class Direction { Up, Down, Left, Right };

// --- Параметры сложности ---
struct DifficultyParams { float delay; int points; };
const std::vector<DifficultyParams> difficulties = {
    {0.20f, 2}, {0.16f, 4}, {0.12f, 6}, {0.08f, 8}, {0.05f, 10}
};
int currentDifficulty = 0;

// --- Таблица рекордов ---
using ScoreEntry = std::pair<std::string, int>;
std::vector<ScoreEntry> highscores;
void loadHighscores() {
    highscores.clear();
    std::ifstream in(RES + "highscores.txt");
    if (!in.is_open()) return;
    std::string name; int score;
    while (in >> name >> score)
    {
        highscores.emplace_back(name, score);
        std::sort(highscores.begin(), highscores.end(),
            [](auto& a, auto& b) { return a.second > b.second; });
    }
}
void saveHighscores()
{
    std::ofstream out(RES + "highscores.txt");
    for (size_t i = 0; i < highscores.size() && i < MAX_MENU_ENTRIES; ++i)
        out << highscores[i].first << " " << highscores[i].second << "\n";
}


struct Segment
{
    sf::Vector2i pos;
    Direction    dir;
};

class Snake
{
public:
    Snake(const sf::Texture& h,
        const sf::Texture& bStraight,
        const sf::Texture& bBend,
        const sf::Texture& t)
        : txHead(h)
        , txBodyStraight(bStraight)
        , txBodyBend(bBend)
        , txTail(t)
    {
        reset();
    }

    void reset()
    {
        segments.clear();
        segments.push_back({ {GRID_WIDTH / 2, GRID_HEIGHT / 2}, Direction::Right });
        segments.push_back({ {GRID_WIDTH / 2 - 1, GRID_HEIGHT / 2}, Direction::Right });
        segments.push_back({ {GRID_WIDTH / 2 - 2, GRID_HEIGHT / 2}, Direction::Right });
    }

    void setDirection(Direction d)
    {
        Direction cur = segments[0].dir;
        if ((cur == Direction::Up && d == Direction::Down) ||
            (cur == Direction::Down && d == Direction::Up) ||
            (cur == Direction::Left && d == Direction::Right) ||
            (cur == Direction::Right && d == Direction::Left))
            return;
        segments[0].dir = d;
    }

    void update(bool grow = false)
    {
        // Запоминаем старые позиции и направления
        std::vector<sf::Vector2i> oldPos; oldPos.reserve(segments.size());
        std::vector<Direction>    oldDir; oldDir.reserve(segments.size());
        for (auto& s : segments) {
            oldPos.push_back(s.pos);
            oldDir.push_back(s.dir);
        }

        // Двигаем голову
        auto& head = segments[0];
        switch (head.dir) {
        case Direction::Up:    --head.pos.y; break;
        case Direction::Down:  ++head.pos.y; break;
        case Direction::Left:  --head.pos.x; break;
        case Direction::Right: ++head.pos.x; break;
        }

        // Остальные сегменты копируют предшественников
        for (size_t i = 1; i < segments.size(); ++i) {
            segments[i].pos = oldPos[i - 1];
            segments[i].dir = oldDir[i - 1];
        }

        // Если съели яблоко — добавляем новый сегмент в хвост
        if (grow) {
            segments.push_back({ oldPos.back(), oldDir.back() });
        }
    }

    bool isDead() const
    {
        auto h = segments.front().pos;
        if (h.x <= 0 || h.x >= GRID_WIDTH - 1 || h.y <= 0 || h.y >= GRID_HEIGHT - 1) return true;
        for (size_t i = 1; i < segments.size(); ++i)
            if (segments[i].pos == h) return true;
        return false;
    }

    const sf::Vector2i& headPos() const { return segments.front().pos; }

    void draws(sf::RenderWindow& wnd)
    {
        // Голова
        drawSeg(wnd, segments.front(), txHead);

        // Тело + хвост
        for (size_t i = 1; i < segments.size(); ++i) {
            // Если это не последний сегмент — это тело
            if (i + 1 < segments.size()) {
                const auto& seg = segments[i];
                Direction prev = seg.dir;
                Direction next = segments[i + 1].dir;
                if (prev == next) {
                    drawBodyStraight(wnd, seg, prev);
                }
                else {
                    drawBodyBend(wnd, seg, prev, next);
                }
            }
            else {
                // Хвост
                drawSeg(wnd, segments.back(), txTail);
            }
        }
    }

private:
    void drawSeg(sf::RenderWindow& wnd, const Segment& s, const sf::Texture& tx)
    {
        sf::Sprite spr(tx);
        auto bounds = spr.getLocalBounds();
        spr.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        spr.setPosition(s.pos.x * CELL_SIZE + CELL_SIZE / 2.f,
            s.pos.y * CELL_SIZE + CELL_SIZE / 2.f);
        float angle = 0;
        switch (s.dir)
        {
        case Direction::Up:    angle = 270; break;
        case Direction::Down:  angle = 90; break;
        case Direction::Left:  angle = 180; break;
        case Direction::Right: angle = 0; break;
        }
        spr.setRotation(angle);
        wnd.draw(spr);
    }

    void drawBodyStraight(sf::RenderWindow& wnd, const Segment& s, Direction dir)
    {
        drawSeg(wnd, s, txBodyStraight);
    }

    void drawBodyBend(sf::RenderWindow& wnd, const Segment& s, Direction prev, Direction next)
    {
        sf::Sprite spr(txBodyBend);
        auto bounds = spr.getLocalBounds();
        spr.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
        spr.setPosition(s.pos.x * CELL_SIZE + CELL_SIZE / 2.f,
            s.pos.y * CELL_SIZE + CELL_SIZE / 2.f);
        float angle = 0;
        // Подбираем угол в зависимости от двух соседних направлений
        if ((prev == Direction::Up && next == Direction::Right) ||
            (prev == Direction::Left && next == Direction::Down))
            angle = 0;
        else if ((prev == Direction::Right && next == Direction::Down) ||
            (prev == Direction::Up && next == Direction::Left))
            angle = 90;
        else if ((prev == Direction::Down && next == Direction::Left) ||
            (prev == Direction::Right && next == Direction::Up))
            angle = 180;
        else
            angle = 270;
        spr.setRotation(angle);
        wnd.draw(spr);
    }

    std::vector<Segment> segments;
    const sf::Texture& txHead;
    const sf::Texture& txBodyStraight;
    const sf::Texture& txBodyBend;
    const sf::Texture& txTail;
};



int main()
{
    std::srand(unsigned(std::time(nullptr)));
    sf::RenderWindow window({ WINDOW_WIDTH, WINDOW_HEIGHT }, "Snake");
    window.setFramerateLimit(60);

    // --- ресурсы ---
    sf::Font    font;  assert(font.loadFromFile(RES + "Fonts/Roboto-Regular.ttf"));
    sf::Texture headT, bodyStraightT, bodyBendT, tailT, appleT, brickT;
    assert(headT.loadFromFile(RES + "snake/head_right.png"));
    assert(bodyStraightT.loadFromFile(RES + "snake/body_horizontal.png"));
    assert(bodyBendT.loadFromFile(RES + "snake/Bodybend.png"));
    assert(tailT.loadFromFile(RES + "snake/tail_left.png"));
    assert(appleT.loadFromFile(RES + "apple.png"));
    assert(brickT.loadFromFile(RES + "pink_brick.png"));

    sf::SoundBuffer eatBuf, dieBuf, bgBuf, enterBuf, hoverBuf;
    assert(eatBuf.loadFromFile(RES + "Snake_hit.wav"));
    assert(dieBuf.loadFromFile(RES + "Lose.wav"));
    assert(bgBuf.loadFromFile(RES + "Background_Music.wav"));
    assert(enterBuf.loadFromFile(RES + "Enter.wav"));
    assert(hoverBuf.loadFromFile(RES + "menu-hover.wav"));

    sf::Sound eatSound(eatBuf), dieSound(dieBuf),
        bgSound(bgBuf), enterSound(enterBuf), hoverSound(hoverBuf);
    bgSound.setLoop(true);

    // init
    GameState state = GameState::Menu;
    Snake snake(headT, bodyStraightT, bodyBendT, tailT);
    sf::Vector2i applePos;
    sf::Sprite appleSpr(appleT);
    appleSpr.setOrigin(CELL_SIZE / 2.f, CELL_SIZE / 2.f);
    int score = 0;
    bool soundOn = true, musicOn = true;
    loadHighscores();

    std::string playerName;

    auto spawnApple = [&]()
        {
            do 
            {
                applePos = { std::rand() % GRID_WIDTH, std::rand() % GRID_HEIGHT };
            } while (applePos.x <= 0 || applePos.x >= GRID_WIDTH - 1 ||
                applePos.y <= 0 || applePos.y >= GRID_HEIGHT - 1 ||
                applePos == snake.headPos());
            appleSpr.setPosition
            (
                applePos.x * CELL_SIZE + CELL_SIZE / 2.f,
                applePos.y * CELL_SIZE + CELL_SIZE / 2.f
            );
        };

    int menuSel = 0, diffSel = 0, settingsSel = 0, deathSel = 0;
    std::vector<std::string> menuOpts = { "Start","Difficulty","High Scores","Settings","Exit" },
        diffOpts = { "1 Simple","2 Easy","3 Medium","4 Hard","5 Very Hard" },
        settingsOpts = { "Sound: ","Music: ","Back" },
        deathOpts = { "Play Again","Menu" };
    auto drawMenu = [&](auto& opts, int sel)
        {
        sf::Text menutext; menutext.setFont(font); menutext.setCharacterSize(24);
        float y = 180;
        for (size_t i = 0; i < opts.size(); ++i) {
            menutext.setString(opts[i]);
            menutext.setPosition(100, y);
            menutext.setFillColor(i == sel ? sf::Color::Green : sf::Color::White);
            window.draw(menutext);
            y += 40;
        }
        };

    sf::Clock moveClock;

    // главный цикл
    while (window.isOpen())
    {
        sf::Event ev;
        while (window.pollEvent(ev)) 
        {
            if (ev.type == sf::Event::Closed) window.close();
            if (ev.type == sf::Event::TextEntered && state == GameState::Death)
            {
                char playerchar = static_cast<char>(ev.text.unicode);
                if (playerchar == '\b' && !playerName.empty()) playerName.pop_back();
                else if (std::isprint(playerchar) && playerName.size() < 3) playerName.push_back(playerchar);
            }
            if (ev.type == sf::Event::KeyPressed)
            {
                switch (state)
                {
                case GameState::Menu:
                    if (ev.key.code == sf::Keyboard::Up)
                    {
                        int m = static_cast<int>(menuOpts.size());
                        menuSel = (menuSel+ m -1) % m;
                        if (soundOn) hoverSound.play();
                    }
                    if (ev.key.code == sf::Keyboard::Down)
                    {
                        menuSel = (menuSel + 1) % menuOpts.size();
                        if (soundOn) hoverSound.play();
                    }
                    if (ev.key.code == sf::Keyboard::Enter)
                    {
                        if (soundOn) enterSound.play();
                        switch (menuSel) {
                        case 0:
                            state = GameState::Playing; snake.reset(); score = 0;
                            currentDifficulty = diffSel;
                            moveClock.restart();
                            if (musicOn) bgSound.play();
                            spawnApple();
                            break;
                        case 1: state = GameState::Difficulty; break;
                        case 2: state = GameState::Highscores; break;
                        case 3: state = GameState::Settings;    break;
                        case 4: window.close();               break;
                        }
                    }
                    break;
                case GameState::Difficulty:
                    if (ev.key.code == sf::Keyboard::Up) 
                    {
                        int d = static_cast<int>(diffOpts.size());
                        diffSel = (diffSel + d - 1) % d;
                        if (soundOn) hoverSound.play();
                    }
                    if (ev.key.code == sf::Keyboard::Down)
                    {
                        diffSel = (diffSel + 1) % diffOpts.size();
                        if (soundOn) hoverSound.play();
                    }
                    if (ev.key.code == sf::Keyboard::Enter)
                    {
                        if (soundOn) enterSound.play();
                        currentDifficulty = diffSel;
                        state = GameState::Menu;
                    }
                    break;
                case GameState::Settings:
                    if (ev.key.code == sf::Keyboard::Up) 
                    {
                        int s = static_cast<int>(settingsOpts.size());
                        settingsSel = (settingsSel + s - 1) % s;
                        if (soundOn) hoverSound.play();
                    }
                    if (ev.key.code == sf::Keyboard::Down) {
                        settingsSel = (settingsSel + 1) % settingsOpts.size();
                        if (soundOn) hoverSound.play();
                    }
                    if (ev.key.code == sf::Keyboard::Enter) 
                    {
                        if (soundOn) enterSound.play();
                        if (settingsSel == 0) soundOn = !soundOn;// ивертирем значение 
                        else if (settingsSel == 1) 
                        {
                            musicOn = !musicOn; // ивертирем значение 
                            if (musicOn) bgSound.play(); else bgSound.stop();
                        }
                        else state = GameState::Menu;
                    }
                    break;
                case GameState::Highscores:
                    if (ev.key.code == sf::Keyboard::B) state = GameState::Menu;
                    break;
                case GameState::Playing:
                    if (ev.key.code == sf::Keyboard::W) snake.setDirection(Direction::Up);
                    if (ev.key.code == sf::Keyboard::S) snake.setDirection(Direction::Down);
                    if (ev.key.code == sf::Keyboard::A) snake.setDirection(Direction::Left);
                    if (ev.key.code == sf::Keyboard::D) snake.setDirection(Direction::Right);
                    if (ev.key.code == sf::Keyboard::P) {
                        state = GameState::Paused;
                        bgSound.pause();
                    }
                    break;
                case GameState::Paused:
                    if (ev.key.code == sf::Keyboard::Enter) {
                        state = GameState::Playing;
                        if (musicOn) bgSound.play();
                    }
                    if (ev.key.code == sf::Keyboard::B) {
                        state = GameState::Menu;
                        bgSound.stop();
                    }
                    break;
                case GameState::Death:
                    if (ev.key.code == sf::Keyboard::Enter)
                    {
                        if (!playerName.empty()) 
                        {
                            highscores.emplace_back(playerName, score);
                            std::sort(highscores.begin(), highscores.end(),
                                [](auto& a, auto& b) { return a.second > b.second; });
                            if (highscores.size() > MAX_MENU_ENTRIES)
                                highscores.resize(MAX_MENU_ENTRIES);
                            saveHighscores();
                        }
                        if (deathSel == 0) 
                        {
                            state = GameState::Playing;
                            if (musicOn) bgSound.play();
                            snake.reset(); score = 0;
                           
                        }
                        else state = GameState::Menu;
                    }
                    if (ev.key.code == sf::Keyboard::Up) {
                        int d = static_cast<int>(deathOpts.size());
                        deathSel = (deathSel + d - 1) %d;
                        if (soundOn) hoverSound.play();
                    }
                    if (ev.key.code == sf::Keyboard::Down) {
                        deathSel = (deathSel + 1) % deathOpts.size();
                        if (soundOn) hoverSound.play();
                    }
                    break;
                }
            }
        }

        // движение
        if (state == GameState::Playing &&
            moveClock.getElapsedTime().asSeconds() >= difficulties[currentDifficulty].delay)
        {
            moveClock.restart();
            bool ate = (snake.headPos() == applePos);
            snake.update(ate);
            if (ate)
            {
                score += difficulties[currentDifficulty].points;
                if (soundOn) eatSound.play();
                spawnApple();
            }
            if (snake.isDead()) 
            {
                state = GameState::Death;
                playerName.clear();
                if (soundOn) dieSound.play();
                bgSound.stop();
            }
        }

        // отрисовка
        window.clear();
        switch (state)
        {
        case GameState::Menu:
            drawMenu(menuOpts, menuSel);
            break;
        case GameState::Difficulty:
            drawMenu(diffOpts, diffSel);
            break;
        case GameState::Settings: {
            auto o = settingsOpts;
            o[0] += (soundOn ? "On" : "Off");
            o[1] += (musicOn ? "On" : "Off");
            drawMenu(o, settingsSel);
        } break;
        case GameState::Highscores: {
            sf::Text HighScores; HighScores.setFont(font); HighScores.setCharacterSize(24);
            float y = 120;
            for (size_t i = 0; i < highscores.size() && i < MAX_MENU_ENTRIES; ++i) {
                HighScores.setString(highscores[i].first + " " + std::to_string(highscores[i].second));
                HighScores.setPosition(100, y);
                window.draw(HighScores);
                y += 30;
            }
            HighScores.setString("Press B to go back");
            HighScores.setPosition(100, y + 20);
            window.draw(HighScores);
        } break;
        case GameState::Playing: {
            sf::Sprite brick(brickT);
            for (int x = 0; x < WINDOW_WIDTH; x += CELL_SIZE) {
                brick.setPosition(float(x), 0.f); window.draw(brick);
                brick.setPosition(float(x), float(WINDOW_HEIGHT - CELL_SIZE)); window.draw(brick);
            }
            for (int y = 0; y < WINDOW_HEIGHT; y += CELL_SIZE) {
                brick.setPosition(0.f, float(y)); window.draw(brick);
                brick.setPosition(float(WINDOW_WIDTH - CELL_SIZE), float(y)); window.draw(brick);
            }
            window.draw(appleSpr);
            snake.draws(window);
            sf::Text ts("Score: " + std::to_string(score), font, 24);
            ts.setFillColor(sf::Color::White);
            ts.setPosition(10, 10);
            window.draw(ts);
        } break;
        case GameState::Paused: {
            sf::Text p("PAUSED", font, 36);
            p.setFillColor(sf::Color::White);
            p.setPosition(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 - 20);
            window.draw(p);
            sf::Text h("Enter=Resume  B=Menu", font, 24);
            h.setFillColor(sf::Color::White);
            h.setPosition(WINDOW_WIDTH / 2 - 140, WINDOW_HEIGHT / 2 + 20);
            window.draw(h);
        } break;
        case GameState::Death: {
            drawMenu(deathOpts, deathSel);
            sf::Text pr("Enter initials: " + playerName, font, 24);
            pr.setFillColor(sf::Color::White);
            pr.setPosition(200, 300);
            window.draw(pr);
            sf::Text t; t.setFont(font); t.setCharacterSize(24);
            float y = 350;
            for (size_t i = 0; i < highscores.size() && i < MAX_POPUP_ENTRIES; ++i) {
                t.setString(highscores[i].first + " " + std::to_string(highscores[i].second));
                t.setPosition(300, y);
                window.draw(t);
                y += 30;
            }
        } break;
        }
        window.display();
    }

    return 0;
}
