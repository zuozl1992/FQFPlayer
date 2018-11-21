#include "myset.h"

MySet::MySet()
{

}

QString MySet::at(int index)
{
    MySet::const_iterator i;
    int t = 0;
    for(i = this->begin(); i != this->end(); i++)
    {
        if(t++ == index)
            return (*i);
    }
    return QString();
}
