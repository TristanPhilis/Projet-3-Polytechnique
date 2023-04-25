# STAGE 1: Build Angular Application
FROM node:lts as build
WORKDIR /app
ARG UPDATE_CODE=unknown
COPY package.json .
RUN npm install
COPY . .
RUN npm run build

# STAGE 2: Deploy with NGINX
FROM nginx:alpine
EXPOSE 80
COPY --from=build /app/dist/client /usr/share/nginx/html
