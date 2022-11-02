#ifndef LOCK_H
#define LOCK_H

class Lock {       // The class
    private:             // Access specifier
        bool lock;

    public: 
        Lock();
        
        void acquire();
        void release();       
};

#endif