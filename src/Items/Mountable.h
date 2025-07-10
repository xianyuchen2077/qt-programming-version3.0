#ifndef QT_PROGRAMMING_2024_MOUNTABLE_H
#define QT_PROGRAMMING_2024_MOUNTABLE_H


class Mountable
{
public:
    virtual void mountToParent(); // 穿戴

    virtual void unmount(); // 脱下

    [[nodiscard]] bool isMounted() const;

private:
    bool mounted{};
};


#endif //QT_PROGRAMMING_2024_MOUNTABLE_H
