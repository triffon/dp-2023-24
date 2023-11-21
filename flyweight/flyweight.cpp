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

// абстракция за реализации
class ConcreteToken {
public:
    virtual std::string toString() const = 0;
    virtual void print(Context const& context) const = 0;
    virtual size_t length() const { return toString().length(); }
};

// абстракция за абстракции
class Token {
protected:
    std::shared_ptr<ConcreteToken> concreteToken;
public:
    Token() = delete;
    Token(std::shared_ptr<ConcreteToken> concreteToken) : concreteToken(concreteToken) {}
    virtual size_t length() const {
        return concreteToken->length();
    }
    virtual void print(Context const& context) const {
        concreteToken->print(context);
    }
    virtual ~Token() {}
};

// конкретизирана абстракция
class TokenWithID : public Token {
protected:
    unsigned id;
public:
    TokenWithID(std::shared_ptr<ConcreteToken> concreteToken) : Token(concreteToken), id(rand()) {}
    virtual void print(Context const& context) const {
        std::cout << "{" << std::to_string(id) + "} ";
        Token::print(context);
    }

};

// конкретна реализация
class NumberToken : public ConcreteToken {
    ushort number;
public:
    NumberToken(ushort number) : number(number) {}
    void print(Context const& context) const {
        std::cout << "Това е числото " << number << " на позиция " << context.getPosition();
    }
    std::string toString() const {
        return std::to_string(number);
    }
};

// конкретна реализация
class WordToken : public ConcreteToken {
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
        if (numberTokens.count(num) == 0) {
            numberTokens[num] = std::make_shared<NumberToken>(num);
        }
        return std::make_shared<Token>(numberTokens[num]);
    }

    std::shared_ptr<Token> getWordToken(std::string word, size_t wordStart) {
        if (word.length() > 5)
            return std::make_shared<TokenWithID>(std::make_shared<WordToken>(word, wordStart));
        else
            return std::make_shared<Token>(std::make_shared<WordToken>(word, wordStart));
    }

};

class Sentence {
    size_t length;
    std::vector<std::shared_ptr<Token>> tokens;
    std::shared_ptr<TokenFactory> tokenFactory;
public:
    Sentence(std::shared_ptr<TokenFactory> tokenFactory) : length(0), tokenFactory(tokenFactory) {}
    Sentence& addWord(std::string word) {
        tokens.push_back(tokenFactory->getWordToken(word, length));
        length += tokens.back()->length();
        return *this;
    }
    Sentence& addNumber(ushort num) {
        tokens.push_back(tokenFactory->getNumberToken(num));
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
    std::shared_ptr<TokenFactory> tokenFactory = std::make_shared<TokenFactory>();
    Sentence sentence(tokenFactory);
    sentence.addWord("Today").addWord("November").addNumber(21).addWord("at").addNumber(13)
            .addWord("hours").addWord("and").addNumber(21).addWord("minutes");
    sentence.print();
    tokenFactory.reset();
    sentence.print();
}
