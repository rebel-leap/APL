#pragma once
#include "apl/common.h"

namespace apl {
    enum class TokenType {
        NUMBER, STRING, BOOLEAN, NIL,
        
        VAR, FUNC, RETURN, IF, ELSE, ELIF, WHILE, FOR, IN, IMPORT, 
        CLASS, NEW, THIS, EXTENDS,
        PRINT, PRINTLN, INPUT, INPUT_LINE, LEN, TYPE,
        TRY, CATCH, FINALLY, THROW,
        LAMBDA, IS, ISNOT, NAMESPACE, STRUCT, MAP, LIST, DICT, SET,
        END, END_FUNC, END_CLASS, END_NAMESPACE,
        
        PLUS, MINUS, STAR, SLASH, PERCENT, POWER,
        SQRT, SIN, COS, TAN, ABS, ROUND, LOG, EXP,
        
        ASSIGN, PLUS_ASSIGN, MINUS_ASSIGN, STAR_ASSIGN, SLASH_ASSIGN, PERCENT_ASSIGN,
        
        EQ, NE, LT, GT, LE, GE,
        AND, OR, NOT,
        
        LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
        COMMA, DOT, COLON, SEMICOLON, ARROW, PLUS_PLUS,
        
        IDENTIFIER, COMMENT, NEWLINE, EOF_TOKEN, UNKNOWN,
        
        NEURON, LAYER, NETWORK, TRAIN, PREDICT, LOSS,
        LEARNING_RATE, BATCH, EPOCH, ACTIVATION, NORMALIZE, GRADIENT,
        
        CREATE_SERVER, SEND_DATA, RECEIVE_DATA,
        
        ADD_MENU, ADD_TEXTBOX, ADD_SCROLLBAR, ADD_BUTTON, ADD_IMAGE, RUN_UI,
        
        RANGE, FILTER, REDUCE,
        
        CREATE_WINDOW, DRAW_TRIANGLE, DRAW_RECT, DRAW_CIRCLE, DRAW_TEXT,
        DRAW_CUBE, DRAW_SPHERE, DRAW_PYRAMID,
        CREATE_CAMERA, MOVE_CAMERA, ROTATE_CAMERA,
        CREATE_LIGHT, CREATE_TEXTURE,
        CREATE_MODEL, LOAD_MODEL, DRAW_MODEL,
        ADD_EFFECT, BACKGROUND, COLOR, POSITION,
        MOVE_OBJECT, ROTATE_OBJECT, CHANGE_COLOR,
        
        CREATE_WORLD, ADD_BODY, ADD_RIGID_BODY, ADD_STATIC_BODY,
        ADD_BOX_SHAPE, ADD_SPHERE_SHAPE, ADD_CYLINDER_SHAPE, ADD_CONE_SHAPE,
        MASS, FRICTION, RESTITUTION, VELOCITY, FORCE, IMPULSE,
        PHYSICS_STEP, GRAVITY, COLLISION,
        
        LOAD_SOUND, PLAY_SOUND, STOP_SOUND, STOP_ALL_SOUNDS,
        VOLUME, PITCH, LOOP_SOUND,
        SOUND_POSITION, SOUND_DISTANCE, SOUND_DIRECTION,
        ECHO_EFFECT, DISTORTION_EFFECT, DELAY_EFFECT,
        RECORD_SOUND, BACKGROUND_MUSIC, SOUND_EFFECTS
    };

    struct Token {
        TokenType type;
        String lexeme;
        size_t line;
        size_t column;
    };

    class Lexer {
    public:
        explicit Lexer(const String& source);
        Vector<Token> tokenize();
    private:
        String source;
        size_t pos = 0;
        size_t line = 1;
        size_t column = 1;
        char peek(size_t offset = 0) const;
        char advance();
        bool match(char expected);
        void skipWhitespace();
        void skipComment();
        Token makeToken(TokenType type, const String& lexeme);
        Token makeToken(TokenType type);
        void stringLiteral();
        void numberLiteral();
        void identifier();
        static bool isArabicLetter(char c);
        static bool isIdentifierStart(char c);
        static bool isIdentifierPart(char c);
        Vector<Token> tokens;
    };
}