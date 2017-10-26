#ifndef ENCODER_H
#define ENCODER_H

enum EncryptionMethods{
    ONE_TIME_PAD
};
class Encoder{
    public:
        Encoder();
//        auto encode(auto content,EncryptionMethods method);
        virtual ~Encoder();

    protected:


    private:
};

#endif // ENCODER_H
