sudo date

sudo docker build -t hl_cmake_ready .

sudo docker tag hl_cmake_ready stor.highloadcup.ru/travels/coati_shooter
sudo docker push stor.highloadcup.ru/travels/coati_shooter