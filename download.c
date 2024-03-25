//URL ve thread sayýsý ile dosya indirme C kodu
//  Format --> yourapp.exe  https://url.link   4(thread number)

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <curl/curl.h>

// Struct to hold thread-specific data
struct ThreadData {
    int thread_id;
    char *url;
    long start_byte;
    long end_byte;
};

// Function to write received data to file
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fwrite(ptr, size, nmemb, stream);
}

// Function to download a portion of the file using a thread
void *download_part(void *arg) {
    struct ThreadData *data = (struct ThreadData *)arg;

    // Create file name with thread ID
    char filename[50];
    sprintf(filename, "part%d.tmp", data->thread_id);

    // Open file for writing in binary mode
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening file");
        pthread_exit(NULL);
    }

    // Initialize libcurl
    CURL *curl = curl_easy_init();
    if (curl) {
        // Set URL for the range of bytes to download
        char range[50];
        sprintf(range, "%ld-%ld", data->start_byte, data->end_byte);
        curl_easy_setopt(curl, CURLOPT_URL, data->url);
        curl_easy_setopt(curl, CURLOPT_RANGE, range);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

        // Perform the request
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Clean up
        curl_easy_cleanup(curl);
    }

    // Close file
    fclose(file);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <URL> <Thread number>\n", argv[0]);
        return 1;
    }

    char *url = argv[1];
    int num_threads = atoi(argv[2]);

    if (num_threads <= 0) {
        fprintf(stderr, "Invalid thread number\n");
        return 1;
    }

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);

    // Create thread IDs array
    pthread_t threads[num_threads];

    // Calculate ranges for each thread
    long file_size = 0;
    {
        CURL *curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url);
            curl_easy_setopt(curl, CURLOPT_NOBODY, 1L); // Get header only
            CURLcode res = curl_easy_perform(curl);
            if (res == CURLE_OK) {
                double content_length;
                curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &content_length);
                file_size = (long)content_length;
            } else {
                fprintf(stderr, "Error getting file size: %s\n", curl_easy_strerror(res));
                curl_easy_cleanup(curl);
                return 1;
            }
            curl_easy_cleanup(curl);
        } else {
            fprintf(stderr, "Error initializing curl\n");
            return 1;
        }
    }

    long part_size = file_size / num_threads;
    struct ThreadData data[num_threads];

    // Create threads
    for (int i = 0; i < num_threads; i++) {
        data[i].thread_id = i + 1;
        data[i].url = url;
        data[i].start_byte = i * part_size;
        data[i].end_byte = (i == num_threads - 1) ? file_size - 1 : (i + 1) * part_size - 1;

        pthread_create(&threads[i], NULL, download_part, (void *)&data[i]);
    }

    // Join threads
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Merge downloaded parts into a single file
    FILE *output_file = fopen("downloaded_file.mp3", "wb");
    if (!output_file) {
        perror("Error opening output file");
        return 1;
    }

    for (int i = 0; i < num_threads; i++) {
        char filename[50];
        sprintf(filename, "part%d.tmp", i + 1);
        FILE *part_file = fopen(filename, "rb");
        if (!part_file) {
            perror("Error opening part file");
            return 1;
        }

        char buffer[1024];
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), part_file)) > 0) {
            fwrite(buffer, 1, bytes_read, output_file);
        }

        fclose(part_file);
        remove(filename);
    }

    fclose(output_file);

    // Clean up libcurl
    curl_global_cleanup();

    printf("Download complete.\n");

    return 0;
}

