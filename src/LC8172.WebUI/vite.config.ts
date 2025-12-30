import { defineConfig } from 'vite';
import vue from '@vitejs/plugin-vue';
import { fileURLToPath, URL } from 'node:url';

// https://vite.dev/config/
export default defineConfig({
  plugins: [vue()],
    base: './', // 使用相对路径
    resolve: {
        alias: {
            '@': fileURLToPath(new URL('./src', import.meta.url))
        }
    },
    build: {
        outDir: '../LC8172.WpfHost/wwwroot', // 构建到WPF项目的wwwroot目录
        emptyOutDir: true,
    },
});
