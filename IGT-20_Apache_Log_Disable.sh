find /etc/apache2/sites-enabled/* -exec sed -i 's/#*[Cc]ustom[Ll]og/#CustomLog/g' {} \;
find /etc/apache2/sites-enabled/* -exec sed -i 's/#*[Ee]rror[Ll]og/#ErrorLog/g' {} \;
sed -i 's/#*ErrorLog ${APACHE_LOG_DIR}\/error.log/#ErrorLog ${APACHE_LOG_DIR}\/error.log\nErrorLog \/dev\/null/g' /etc/apache2/apache2.conf
