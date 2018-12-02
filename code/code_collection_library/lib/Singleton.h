
// Copyright (C) 2018 qmsggg

#ifndef SINGLETON_H
#define SINGLETON_H
 
template <class T> class Singleton
{
public:
    Singleton<T>(T *obj)
    {
        instance = obj;
    }
    static T* instance;
};

template <class T> T* Singleton<T>::instance = 0;
 
#endif /* SINGLETON_H */
