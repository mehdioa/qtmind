#ifndef IPEGCONNECTOR_H
#define IPEGCONNECTOR_H

class Peg;

class IPegConnector
{
public:
    virtual void connectPegToGame(Peg*) = 0;
};

#endif // IPEGCONNECTOR_H
