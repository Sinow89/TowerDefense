
struct myStruct {
    int a;
    int b;
};

void myfunc(int& myintreference){
}

int sumStruct(myStruct &my) {
    return my.a + my.b;
}

void main() {
    int num = 5;
    int *pointer_to_num = &num;

    myfunc(num);
}