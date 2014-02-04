/* 
 * File:   Target.h
 * Author: rswhiting
 *
 * Created on February 3, 2014, 9:22 PM
 */

#ifndef TARGET_H
#define	TARGET_H

class Target {
public:
    Target();
    virtual ~Target();

    int getX() const {
        return x;
    }

    void setX(int x) {
        this->x = x;
    }

    int getY() const {
        return y;
    }

    void setY(int y) {
        this->y = y;
    }
private:
    int x;
    int y;

};

#endif	/* TARGET_H */

