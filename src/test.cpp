for (int row = 0; row < N; row++) {
    for (int col = 0; col < N; col++) {
        int r = 0;
        for (int k = 0; k < N; k++) {
            r += a[row][k] * b[k][col];
        }
        x[row][j] = r;
    }
}
