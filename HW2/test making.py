with open("C:/Users/WIN10PRO/Desktop/My Stuff/University/BSC/Operating Systems/Homework/HW2/test_example.in",'w') as f:
    f.write('create_list\n')
    # inserting values 0-1000
    for i in range(0,101):
        f.write('insert_value '+str(i)+'\n')
    # prints 500
    f.write('count_greater 50\n')
    for i in range(0,101):
        f.write('remove_value '+str(i)+'\n')
        f.write('insert_value '+str(i)+'\n')
    # prints 500
    f.write('count_greater 50\n')
    for i in range(20,101):
        f.write('remove_value '+str(i)+'\n')
    f.write('print_list\n')
    for i in range(11,19):
        f.write('remove_value '+str(i)+'\n')
    f.write('print_list\n')
    f.write('delete_list\n')
    f.write('join\n')
    f.write('exit\n')
print('done!')

