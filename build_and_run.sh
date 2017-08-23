sudo docker build -t hl_cmake_ready .
sudo docker run --rm -p 80:80 -v/home/dragoon/IdeaProjects/highload/data:/tmp/data -t hl_cmake_ready
