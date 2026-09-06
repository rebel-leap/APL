#include "apl/lexer.h"
#include <cstdint>

namespace apl {
    Lexer::Lexer(const String& source) : source(source) {}

    char Lexer::peek(size_t offset) const {
        if (pos + offset >= source.size()) return '\0';
        return source[pos + offset];
    }

    char Lexer::advance() {
        char c = peek();
        pos++;
        if (c == '\n') { line++; column = 1; }
        else { column++; }
        return c;
    }

    bool Lexer::match(char expected) {
        if (peek() != expected) return false;
        advance();
        return true;
    }

    void Lexer::skipWhitespace() {
        while (true) {
            char c = peek();
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') advance();
            else break;
        }
    }

    void Lexer::skipComment() {
        if (peek() == '/' && peek(1) == '/') {
            while (peek() != '\n' && peek() != '\0') advance();
        }
    }

    Token Lexer::makeToken(TokenType type, const String& lexeme) {
        return Token{type, lexeme, line, column};
    }

    Token Lexer::makeToken(TokenType type) {
        return makeToken(type, String(1, peek(-1)));
    }

    bool Lexer::isArabicLetter(char c) {
        unsigned char uc = static_cast<unsigned char>(c);
        return (uc >= 0x80);
    }

    bool Lexer::isIdentifierStart(char c) {
        return std::isalpha(static_cast<unsigned char>(c)) || c == '_' || isArabicLetter(c);
    }

    bool Lexer::isIdentifierPart(char c) {
        return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || isArabicLetter(c);
    }

    void Lexer::stringLiteral() {
        char quote = advance();
        String value;
        while (peek() != quote && peek() != '\0') {
            if (peek() == '\\') {
                advance();
                char esc = advance();
                switch (esc) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '\\': value += '\\'; break;
                    case '"': value += '"'; break;
                    case '\'': value += '\''; break;
                    default: value += esc; break;
                }
            } else {
                value += advance();
            }
        }
        if (peek() == quote) advance();
        tokens.push_back(makeToken(TokenType::STRING, value));
    }

    void Lexer::numberLiteral() {
        String value;
        while (std::isdigit(static_cast<unsigned char>(peek())) || peek() == '.') {
            value += advance();
        }
        tokens.push_back(makeToken(TokenType::NUMBER, value));
    }

    void Lexer::identifier() {
        String value;
        while (isIdentifierPart(peek())) {
            value += advance();
        }

        TokenType type = TokenType::IDENTIFIER;

        // ─── الكلمات الأساسية ───
        if (value == "اطبع") type = TokenType::PRINT;
        else if (value == "اطبع_سطر") type = TokenType::PRINTLN;
        else if (value == "متغير" || value == "var") type = TokenType::VAR;
        else if (value == "إذا" || value == "if") type = TokenType::IF;
        else if (value == "وإلا" || value == "else") type = TokenType::ELSE;
        else if (value == "وإلا_إذا" || value == "elif") type = TokenType::ELIF;
        else if (value == "بينما" || value == "while") type = TokenType::WHILE;
        else if (value == "لكل" || value == "for") type = TokenType::FOR;
        else if (value == "في" || value == "in") type = TokenType::IN;
        else if (value == "أرجع" || value == "return") type = TokenType::RETURN;
        else if (value == "صنف" || value == "class") type = TokenType::CLASS;
        else if (value == "جديد" || value == "new") type = TokenType::NEW;
        else if (value == "هذا" || value == "this") type = TokenType::THIS;
        else if (value == "يمتد" || value == "extends") type = TokenType::EXTENDS;
        else if (value == "صحيح" || value == "true") type = TokenType::BOOLEAN;
        else if (value == "خطأ" || value == "false") type = TokenType::BOOLEAN;
        else if (value == "عدم" || value == "nil") type = TokenType::NIL;
        else if (value == "لا_شيء" || value == "null") type = TokenType::NIL;
        else if (value == "و" || value == "and") type = TokenType::AND;
        else if (value == "أو" || value == "or") type = TokenType::OR;
        else if (value == "ليس" || value == "not") type = TokenType::NOT;
        else if (value == "اقرأ" || value == "input") type = TokenType::INPUT;
        else if (value == "اقرأ_سطر") type = TokenType::INPUT_LINE;
        else if (value == "طول" || value == "len") type = TokenType::LEN;
        else if (value == "نوع" || value == "type") type = TokenType::TYPE;
        else if (value == "استورد" || value == "import") type = TokenType::IMPORT;
        else if (value == "حاول" || value == "try") type = TokenType::TRY;
        else if (value == "امسك" || value == "catch") type = TokenType::CATCH;
        else if (value == "أخيراً" || value == "finally") type = TokenType::FINALLY;
        else if (value == "اقذف" || value == "throw") type = TokenType::THROW;
        else if (value == "دالة" || value == "func") type = TokenType::FUNC;
        else if (value == "لامدا" || value == "lambda") type = TokenType::LAMBDA;
        else if (value == "هو" || value == "is") type = TokenType::IS;
        else if (value == "ليس_هو" || value == "isnot") type = TokenType::ISNOT;
        else if (value == "فضاء" || value == "namespace") type = TokenType::NAMESPACE;
        else if (value == "هيكل" || value == "struct") type = TokenType::STRUCT;
        else if (value == "خريطة" || value == "map") type = TokenType::MAP;
        else if (value == "قائمة" || value == "list") type = TokenType::LIST;
        else if (value == "قاموس" || value == "dict") type = TokenType::DICT;
        else if (value == "مجموعة" || value == "set") type = TokenType::SET;
        else if (value == "نهاية") type = TokenType::END;
        else if (value == "نهاية_دالة") type = TokenType::END_FUNC;
        else if (value == "نهاية_صنف") type = TokenType::END_CLASS;
        else if (value == "نهاية_فضاء") type = TokenType::END_NAMESPACE;

        // ─── الكلمات الرياضية ───
        else if (value == "زائد" || value == "+") type = TokenType::PLUS;
        else if (value == "ناقص" || value == "-") type = TokenType::MINUS;
        else if (value == "ضرب" || value == "*") type = TokenType::STAR;
        else if (value == "قسمة" || value == "/") type = TokenType::SLASH;
        else if (value == "باقي" || value == "%") type = TokenType::PERCENT;
        else if (value == "اس" || value == "^") type = TokenType::POWER;
        else if (value == "جذر" || value == "sqrt") type = TokenType::SQRT;
        else if (value == "جيب" || value == "sin") type = TokenType::SIN;
        else if (value == "جيب_تمام" || value == "cos") type = TokenType::COS;
        else if (value == "ظل" || value == "tan") type = TokenType::TAN;
        else if (value == "قيمة_مطلقة" || value == "abs") type = TokenType::ABS;
        else if (value == "تقريب" || value == "round") type = TokenType::ROUND;
        else if (value == "لوغريتم" || value == "log") type = TokenType::LOG;
        else if (value == "اسي" || value == "exp") type = TokenType::EXP;
        else if (value == "يساوي" || value == "=") type = TokenType::ASSIGN;
        else if (value == "يساوي_يساوي" || value == "==") type = TokenType::EQ;
        else if (value == "لا_يساوي" || value == "!=") type = TokenType::NE;
        else if (value == "أصغر" || value == "<") type = TokenType::LT;
        else if (value == "أكبر" || value == ">") type = TokenType::GT;
        else if (value == "أصغر_يساوي" || value == "<=") type = TokenType::LE;
        else if (value == "أكبر_يساوي" || value == ">=") type = TokenType::GE;
        else if (value == "زائد_يساوي" || value == "+=") type = TokenType::PLUS_ASSIGN;
        else if (value == "ناقص_يساوي" || value == "-=") type = TokenType::MINUS_ASSIGN;
        else if (value == "ضرب_يساوي" || value == "*=") type = TokenType::STAR_ASSIGN;
        else if (value == "قسمة_يساوي" || value == "/=") type = TokenType::SLASH_ASSIGN;
        else if (value == "باقي_يساوي" || value == "%=") type = TokenType::PERCENT_ASSIGN;

        // ─── الذكاء الاصطناعي ───
        else if (value == "عصبون" || value == "neuron") type = TokenType::NEURON;
        else if (value == "طبقة" || value == "layer") type = TokenType::LAYER;
        else if (value == "شبكة" || value == "network") type = TokenType::NETWORK;
        else if (value == "تدريب" || value == "train") type = TokenType::TRAIN;
        else if (value == "توقع" || value == "predict") type = TokenType::PREDICT;
        else if (value == "خسارة" || value == "loss") type = TokenType::LOSS;
        else if (value == "معدل_تعلم" || value == "lr") type = TokenType::LEARNING_RATE;
        else if (value == "دفعة" || value == "batch") type = TokenType::BATCH;
        else if (value == "عصر" || value == "epoch") type = TokenType::EPOCH;
        else if (value == "تنشيط" || value == "activation") type = TokenType::ACTIVATION;
        else if (value == "تطبيع" || value == "normalize") type = TokenType::NORMALIZE;
        else if (value == "تدرج" || value == "gradient") type = TokenType::GRADIENT;

        // ─── الشبكات ───
        else if (value == "انشئ_خادم" || value == "create_server") type = TokenType::CREATE_SERVER;
        else if (value == "ارسل_بيانات" || value == "send_data") type = TokenType::SEND_DATA;
        else if (value == "استقبل_بيانات" || value == "receive_data") type = TokenType::RECEIVE_DATA;

        // ─── الواجهات الرسومية ───
        else if (value == "اضف_قائمة" || value == "add_menu") type = TokenType::ADD_MENU;
        else if (value == "اضف_حقل_نص" || value == "add_textbox") type = TokenType::ADD_TEXTBOX;
        else if (value == "اضف_شريط_تمرير" || value == "add_scrollbar") type = TokenType::ADD_SCROLLBAR;
        else if (value == "اضف_زر" || value == "add_button") type = TokenType::ADD_BUTTON;
        else if (value == "اضف_صورة" || value == "add_image") type = TokenType::ADD_IMAGE;
        else if (value == "شغل_واجهة" || value == "run_ui") type = TokenType::RUN_UI;

        // ─── المساعدة ───
        else if (value == "نطاق" || value == "range") type = TokenType::RANGE;
        else if (value == "فلتر" || value == "filter") type = TokenType::FILTER;
        else if (value == "اختزل" || value == "reduce") type = TokenType::REDUCE;

        // ─── الرسوميات ───
        else if (value == "انشئ_نافذة" || value == "create_window") type = TokenType::CREATE_WINDOW;
        else if (value == "ارسم_مثلث" || value == "draw_triangle") type = TokenType::DRAW_TRIANGLE;
        else if (value == "ارسم_مربع" || value == "draw_rect") type = TokenType::DRAW_RECT;
        else if (value == "ارسم_دائرة" || value == "draw_circle") type = TokenType::DRAW_CIRCLE;
        else if (value == "ارسم_نص" || value == "draw_text") type = TokenType::DRAW_TEXT;
        else if (value == "ارسم_مكعب" || value == "draw_cube") type = TokenType::DRAW_CUBE;
        else if (value == "ارسم_كرة" || value == "draw_sphere") type = TokenType::DRAW_SPHERE;
        else if (value == "ارسم_هرم" || value == "draw_pyramid") type = TokenType::DRAW_PYRAMID;
        else if (value == "انشئ_كاميرا" || value == "create_camera") type = TokenType::CREATE_CAMERA;
        else if (value == "حرك_كاميرا" || value == "move_camera") type = TokenType::MOVE_CAMERA;
        else if (value == "دور_كاميرا" || value == "rotate_camera") type = TokenType::ROTATE_CAMERA;
        else if (value == "انشئ_ضوء" || value == "create_light") type = TokenType::CREATE_LIGHT;
        else if (value == "انشئ_نسيج" || value == "create_texture") type = TokenType::CREATE_TEXTURE;
        else if (value == "انشئ_نموذج" || value == "create_model") type = TokenType::CREATE_MODEL;
        else if (value == "تحميل_نموذج" || value == "load_model") type = TokenType::LOAD_MODEL;
        else if (value == "ارسم_نموذج" || value == "draw_model") type = TokenType::DRAW_MODEL;
        else if (value == "اضف_مؤثر" || value == "add_effect") type = TokenType::ADD_EFFECT;
        else if (value == "خلفية" || value == "background") type = TokenType::BACKGROUND;
        else if (value == "لون" || value == "color") type = TokenType::COLOR;
        else if (value == "موقع" || value == "position") type = TokenType::POSITION;
        else if (value == "حرك_كائن" || value == "move_object") type = TokenType::MOVE_OBJECT;
        else if (value == "دور_كائن" || value == "rotate_object") type = TokenType::ROTATE_OBJECT;
        else if (value == "غير_لون" || value == "change_color") type = TokenType::CHANGE_COLOR;

        // ─── الفيزياء ───
        else if (value == "انشئ_عالم" || value == "create_world") type = TokenType::CREATE_WORLD;
        else if (value == "اضف_جسم" || value == "add_body") type = TokenType::ADD_BODY;
        else if (value == "اضف_جسم_صلب" || value == "add_rigid_body") type = TokenType::ADD_RIGID_BODY;
        else if (value == "اضف_جسم_ثابت" || value == "add_static_body") type = TokenType::ADD_STATIC_BODY;
        else if (value == "اضف_شكل_مكعب" || value == "add_box_shape") type = TokenType::ADD_BOX_SHAPE;
        else if (value == "اضف_شكل_كرة" || value == "add_sphere_shape") type = TokenType::ADD_SPHERE_SHAPE;
        else if (value == "اضف_شكل_اسطوانة" || value == "add_cylinder_shape") type = TokenType::ADD_CYLINDER_SHAPE;
        else if (value == "اضف_شكل_مخروط" || value == "add_cone_shape") type = TokenType::ADD_CONE_SHAPE;
        else if (value == "كتلة" || value == "mass") type = TokenType::MASS;
        else if (value == "احتكاك" || value == "friction") type = TokenType::FRICTION;
        else if (value == "ارتداد" || value == "restitution") type = TokenType::RESTITUTION;
        else if (value == "سرعة" || value == "velocity") type = TokenType::VELOCITY;
        else if (value == "قوة" || value == "force") type = TokenType::FORCE;
        else if (value == "دفعة" || value == "impulse") type = TokenType::IMPULSE;
        else if (value == "حدث_فيزياء" || value == "physics_step") type = TokenType::PHYSICS_STEP;
        else if (value == "جاذبية" || value == "gravity") type = TokenType::GRAVITY;
        else if (value == "تصادم" || value == "collision") type = TokenType::COLLISION;

        // ─── الصوت ───
        else if (value == "تحميل_صوت" || value == "load_sound") type = TokenType::LOAD_SOUND;
        else if (value == "تشغيل_صوت" || value == "play_sound") type = TokenType::PLAY_SOUND;
        else if (value == "ايقاف_صوت" || value == "stop_sound") type = TokenType::STOP_SOUND;
        else if (value == "ايقاف_صوت_كل" || value == "stop_all_sounds") type = TokenType::STOP_ALL_SOUNDS;
        else if (value == "مستوى_صوت" || value == "volume") type = TokenType::VOLUME;
        else if (value == "سرعة_صوت" || value == "pitch") type = TokenType::PITCH;
        else if (value == "دورة_صوت" || value == "loop_sound") type = TokenType::LOOP_SOUND;
        else if (value == "موضع_صوت" || value == "sound_position") type = TokenType::SOUND_POSITION;
        else if (value == "مسافة_صوت" || value == "sound_distance") type = TokenType::SOUND_DISTANCE;
        else if (value == "اتجاه_صوت" || value == "sound_direction") type = TokenType::SOUND_DIRECTION;
        else if (value == "مؤثر_صدى" || value == "echo_effect") type = TokenType::ECHO_EFFECT;
        else if (value == "مؤثر_تشويش" || value == "distortion_effect") type = TokenType::DISTORTION_EFFECT;
        else if (value == "مؤثر_تأخير" || value == "delay_effect") type = TokenType::DELAY_EFFECT;
        else if (value == "تسجيل_صوت" || value == "record_sound") type = TokenType::RECORD_SOUND;
        else if (value == "صوت_خلفية" || value == "background_music") type = TokenType::BACKGROUND_MUSIC;
        else if (value == "مؤثرات_صوتية" || value == "sound_effects") type = TokenType::SOUND_EFFECTS;

        tokens.push_back(makeToken(type, value));
    }

    Vector<Token> Lexer::tokenize() {
        while (pos < source.size()) {
            skipWhitespace();
            skipComment();
            if (pos >= source.size()) break;

            char c = peek();

            if (c == '"' || c == '\'') { stringLiteral(); continue; }
            else if (std::isdigit(static_cast<unsigned char>(c))) { numberLiteral(); continue; }
            else if (isIdentifierStart(c)) { identifier(); continue; }

            switch (c) {
                case '+':
                    advance();
                    if (match('=')) tokens.push_back(makeToken(TokenType::PLUS_ASSIGN, "+="));
                    else if (match('+')) tokens.push_back(makeToken(TokenType::PLUS_PLUS, "++"));
                    else tokens.push_back(makeToken(TokenType::PLUS, "+"));
                    break;
                case '-':
                    advance();
                    if (match('=')) tokens.push_back(makeToken(TokenType::MINUS_ASSIGN, "-="));
                    else if (match('>')) tokens.push_back(makeToken(TokenType::ARROW, "->"));
                    else tokens.push_back(makeToken(TokenType::MINUS, "-"));
                    break;
                case '*':
                    advance();
                    if (match('=')) tokens.push_back(makeToken(TokenType::STAR_ASSIGN, "*="));
                    else if (match('*')) { advance(); tokens.push_back(makeToken(TokenType::POWER, "**")); }
                    else tokens.push_back(makeToken(TokenType::STAR, "*"));
                    break;
                case '/':
                    advance();
                    if (match('=')) tokens.push_back(makeToken(TokenType::SLASH_ASSIGN, "/="));
                    else tokens.push_back(makeToken(TokenType::SLASH, "/"));
                    break;
                case '%':
                    advance();
                    if (match('=')) tokens.push_back(makeToken(TokenType::PERCENT_ASSIGN, "%="));
                    else tokens.push_back(makeToken(TokenType::PERCENT, "%"));
                    break;
                case '=':
                    advance();
                    if (match('=')) tokens.push_back(makeToken(TokenType::EQ, "=="));
                    else tokens.push_back(makeToken(TokenType::ASSIGN, "="));
                    break;
                case '!':
                    advance();
                    if (match('=')) tokens.push_back(makeToken(TokenType::NE, "!="));
                    else tokens.push_back(makeToken(TokenType::NOT, "!"));
                    break;
                case '<':
                    advance();
                    if (match('=')) tokens.push_back(makeToken(TokenType::LE, "<="));
                    else tokens.push_back(makeToken(TokenType::LT, "<"));
                    break;
                case '>':
                    advance();
                    if (match('=')) tokens.push_back(makeToken(TokenType::GE, ">="));
                    else tokens.push_back(makeToken(TokenType::GT, ">"));
                    break;
                case '(': advance(); tokens.push_back(makeToken(TokenType::LPAREN, "(")); break;
                case ')': advance(); tokens.push_back(makeToken(TokenType::RPAREN, ")")); break;
                case '{': advance(); tokens.push_back(makeToken(TokenType::LBRACE, "{")); break;
                case '}': advance(); tokens.push_back(makeToken(TokenType::RBRACE, "}")); break;
                case '[': advance(); tokens.push_back(makeToken(TokenType::LBRACKET, "[")); break;
                case ']': advance(); tokens.push_back(makeToken(TokenType::RBRACKET, "]")); break;
                case ',': advance(); tokens.push_back(makeToken(TokenType::COMMA, ",")); break;
                case '.': advance(); tokens.push_back(makeToken(TokenType::DOT, ".")); break;
                case ':': advance(); tokens.push_back(makeToken(TokenType::COLON, ":")); break;
                case ';': advance(); tokens.push_back(makeToken(TokenType::SEMICOLON, ";")); break;
                case '\n': advance(); tokens.push_back(makeToken(TokenType::NEWLINE, "\n")); break;
                default:
                    advance();
                    tokens.push_back(makeToken(TokenType::UNKNOWN, String(1, c)));
                    break;
            }
        }

        tokens.push_back(Token{TokenType::EOF_TOKEN, "", line, column});
        return tokens;
    }
}