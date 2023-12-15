# only works on bash
# bash 3_rtp_pipeline.sh

# set variable filename
filename=3_rtp_pipeline

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

echo_color blue "Build and run $filename.cc"
g++ $filename.cc -o $filename.out `pkg-config --libs --cflags flexpipe`

echo_color yellow "Run ./$filename"
./$filename.out
