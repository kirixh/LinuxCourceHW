echo "TEST №1 - empty check, empty remove, add, check, remove, check"
echo "checkuser kirill" > /dev/phonebook && cat /dev/phonebook
echo "rmuser kirill" > /dev/phonebook && cat /dev/phonebook
echo "adduser kirill piatk 20 1234 kir@gmail.com" > /dev/phonebook && cat /dev/phonebook
echo "checkuser piatk" > /dev/phonebook && cat /dev/phonebook
echo "rmuser piatk" > /dev/phonebook && cat /dev/phonebook
echo "checkuser piatk" > /dev/phonebook && cat /dev/phonebook

echo "TEST №2 - adding 3 users, 2 of them are 'piatk'. Checking and removing them all. Then final check."
echo "adduser ilya piatk 22 12353145 kiluha@gmail.com" > /dev/phonebook && cat /dev/phonebook
echo "adduser ivan petrov 34 1231234 petrov@gmail.com" > /dev/phonebook && cat /dev/phonebook
echo "adduser kirill piatk 20 1234 kir@gmail.com" > /dev/phonebook && cat /dev/phonebook
echo "checkuser piatk" > /dev/phonebook && cat /dev/phonebook
echo "rmuser piatk" > /dev/phonebook && cat /dev/phonebook
echo "rmuser piatk" > /dev/phonebook && cat /dev/phonebook
echo "rmuser petrov" > /dev/phonebook && cat /dev/phonebook
echo "checkuser piatk" > /dev/phonebook && cat /dev/phonebook