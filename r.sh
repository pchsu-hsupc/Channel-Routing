if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <input number>"
    exit 1
fi

input_number=$1

make clean
make

./Lab4 "./input/input${input_number}.in" "./output/output${input_number}.out"
./testcase/SolutionChecker "./input/input${input_number}.in" "./output/output${input_number}.out"
