# only works on bash
# bash 5_local_shm_pipeline.sh

rm /dev/shm/5_local_shm

filename1=5_local_shm_pipeline_source
filename2=5_local_shm_pipeline_sink

function echo_color()
{
    case $1 in
        red)
            echo -e "\033[31m$2\033[0m"
            ;;
        green)
            echo -e "\033[32m$2\033[0m"
            ;;
        yellow)
            echo -e "\033[33m$2\033[0m"
            ;;
        blue)
            echo -e "\033[34m$2\033[0m"
            ;;
        *)
            echo "Usage: echo_color red|green|yellow|blue string"
            ;;
    esac
}

echo_color blue "Build and run $filename1.cc and $filename2.cc"
g++ $filename1.cc -o $filename1.out `pkg-config --libs --cflags flexpipe`
g++ $filename2.cc -o $filename2.out `pkg-config --libs --cflags flexpipe`

echo_color yellow "Run ./sink & ./source"
./$filename2.out &
./$filename1.out
