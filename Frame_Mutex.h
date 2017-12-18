#include <pthread.h>

class Frame_Mutex
{
public:
    Frame_Mutex(){
        pthread_mutex_init( &m_mutex, NULL );
    }
    void lock() {
        pthread_mutex_lock( &m_mutex );
    }
    void unlock() {
        pthread_mutex_unlock( &m_mutex );
    }


private:
    pthread_mutex_t m_mutex;

};
