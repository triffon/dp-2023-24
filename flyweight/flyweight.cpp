#include <cstddef>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <exception>
#include <memory>

using ushort = unsigned short;

class Context {
    std::size_t position;
public:
    Context(std::size_t position) : position(position) {}
    std::size_t getPosition() const { return position; }
};

class Token {
public:
    virtual size_t length() const = 0;
    virtual void print(Context const& context) const = 0;
    virtual std::string toString() const = 0;
    virtual ~Token() {}
};

class NumberToken : public Token {
    ushort number;
public:
    NumberToken(ushort number) : number(number) {}
    size_t length() const { return std::to_string(number).length(); }
    void print(Context const& context) const {
        std::cout << "Това е числото " << number << " на позиция " << context.getPosition();
    }
    std::string toString() const {
        return std::to_string(number);
    }
};

class WordToken : public Token {
    std::string word;
    size_t wordStart;
public:
    WordToken(std::string word, size_t wordStart) : word(word), wordStart(wordStart) {}
    size_t length() const { return word.length(); }
    void print(Context const& context) const {
        if (context.getPosition() < wordStart || context.getPosition() >= wordStart + length())
            throw std::runtime_error(std::string("Невалиден контекст за думата ") + toString());
        std::cout << "Това е думата " << toString() << " на позиция " << context.getPosition();
    }

    std::string toString() const {
        return word + "(" + std::to_string(wordStart) + ")";
    }
};

class TokenFactory {
    std::map<ushort, std::shared_ptr<NumberToken>> numberTokens;
public:

    std::shared_ptr<Token> getNumberToken(ushort num) {
        if (numberTokens.count(num) == 0)
            numberTokens[num] = std::make_shared<NumberToken>(num);
        return numberTokens[num];
    }

    std::shared_ptr<Token> getWordToken(std::string word, size_t wordStart) {
        return std::make_shared<WordToken>(word, wordStart);
    }
};

class Sentence {
    size_t length;
    std::vector<std::shared_ptr<Token>> tokens;
    TokenFactory& tokenFactory;
public:
    Sentence(TokenFactory& tokenFactory) : length(0), tokenFactory(tokenFactory) {}
    Sentence& addWord(std::string word) {
        tokens.push_back(tokenFactory.getWordToken(word, length));
        length += tokens.back()->length();
        return *this;
    }
    Sentence& addNumber(ushort num) {
        tokens.push_back(tokenFactory.getNumberToken(num));
        length += tokens.back()->length();
        return *this;
    }

    void print() const {        
        for(size_t position = 0, index = 0, currentTokenPosition = 0; position < length; position++, currentTokenPosition++) {
            if (currentTokenPosition >= tokens[index]->length()) {
                currentTokenPosition = 0;
                index++;
            }
            std::cout << "[" << position << "] ";
            Context context(position);
            tokens[index]->print(context);
            std::cout << std::endl;
        }
    }
};

int main(int, char**){
    TokenFactory* tokenFactory = new TokenFactory;
    Sentence sentence(*tokenFactory);
    sentence.addWord("Today").addWord("November").addNumber(21).addWord("at").addNumber(13)
            .addWord("hours").addWord("and").addNumber(21).addWord("minutes");
    sentence.print();
    delete tokenFactory;
    sentence.print();
}
