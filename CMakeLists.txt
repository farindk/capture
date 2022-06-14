cmake_minimum_required(VERSION 3.17)
project(capture)

# Set expiration duration to 0 to switch off expiration
set(EXPIRATION_DURATION_MONTHS 12)

string(TIMESTAMP TODAY "%Y%m%d")

add_subdirectory(recorder)
