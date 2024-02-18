# Instalación del paquete desde un repositorio en caso de no estar instalado
# install.packages("plotly")
# Carga del paquete "plotly" si está instalado
library(plotly)

##########################################################

# EJEMPLO1:


#Definimos la malla cuadrada donde representaremos la normal bivariada
x<-seq(-6,6,length=60);y<-x

# Función Normal Bivariada

f<-function(x,y,mu=c(0,0),s=c(1,0,1))
{
  mu1<-mu[1];mu2<-mu[2]
  s11<-s[1];s12<-s[2];s22<-s[3];rho<-s12/(s11*s22)
  term1<-1/(2*pi*sqrt(s11*s22*(1-rho^2)))
  term2<--1/(2*(1-rho^2))
  term3<-(x-mu1)^2/s11
  term4<-(y-mu2)^2/s22
  term5<--2*rho*((x-mu1)*(y-mu2))/(sqrt(s11)*sqrt(s22))
  term1*exp(term2*(term3+term4-term5))
} 

z<-outer(x,y,f,s=c(1,0,1)) # matriz de densidades

# Primer gráfico de ejemplo

# Este código grafica la superficie dada por la densidad de la norma bivariada

persp(x, y, z,col="lightgrey",theta=30, phi=20,r=50,d=0.1,expand=0.5,ltheta=90,
      main="bivariate normal distribution",lphi=180,shade=0.3,ticktype="detailed",
      nticks=5,cex.axis=0.7,zlab = 'f(x,y)')
mtext(expression(list(mu[x]==0,mu[y]==0,sigma[x]==5,sigma[y]==5,
                      sigma[xy]==0,rho==0)), side=3)

# Segundo gráfico ejemplo
# Este código genera el objeto interactivo
fig <- plot_ly(z = ~z) %>% add_surface(
  contours = list(
    z = list(
      show=TRUE,
      usecolormap=TRUE,
      highlightcolor="#ff0000",
      project=list(z=TRUE)
    )
  )
)
# Modificación de las opciones de visualización
# Consultar la ayuda de "layout" para más información
fig <- fig %>% layout(
  scene = list(
    camera=list(
      eye = list(x=1.87, y=1.88, z=-1.64)
    )
  )
)
# Visualización del objeto interactivo
fig

# Tercer gráfico de ejemplo

# Este código grafica las curvas de nivel de la superficie

require(grDevices); require(graphics)
filled.contour(x = seq(-6,6,length=60), y = seq(-6,6,length=60), z, color.palette = grey.colors, asp = 1)
title(main = "normal data: filled contour map")


############################################

# EJEMPLO 2

#Definimos la malla cuadrada donde representaremos la normal bivariada
#x<-seq(-4,8,length=60);y<-seq(-2,10,length=60)
x<-seq(-10,10,length=60);y<-x

# Función Normal Bivariada

f<-function(x,y,mu=c(2,4),s=c(3,9,6))
{
  mu1<-mu[1];mu2<-mu[2]
  s11<-s[1];s12<-s[2];s22<-s[3];rho<-s12/(s11*s22)
  term1<-1/(2*pi*sqrt(s11*s22*(1-rho^2)))
  term2<--1/(2*(1-rho^2))
  term3<-(x-mu1)^2/s11
  term4<-(y-mu2)^2/s22
  term5<--2*rho*((x-mu1)*(y-mu2))/(sqrt(s11)*sqrt(s22))
  term1*exp(term2*(term3+term4-term5)
} 

z<-outer(x,y,f,s=c(3,9,6)) # matriz de densidades
# Primer gráfico de ejemplo
# Este código grafica la superficie dada por la densidad de la norma bivariada
persp(x, y, z,col="lightgrey",theta=30, phi=20,r=50,d=0.1,expand=0.5,ltheta=90,
      main="bivariate normal distribution",lphi=180,shade=0.3,ticktype="detailed",
      nticks=5,cex.axis=0.7,zlab = 'f(x,y)', lwd = 0.05)
mtext(expression(list(mu[x]==2,mu[y]==4,sigma[x]==3,sigma[y]==6,
                      sigma[xy]==9,rho==0.5)), side=3)


# Segundo gráfico ejemplo
# Este código genera el objeto interactivo
fig <- plot_ly(z = ~z) %>% add_surface(
  contours = list(
    z = list(
      show=TRUE,
      usecolormap=TRUE,
      highlightcolor="#ff0000",
      project=list(z=TRUE)
    )
  )
)
# Modificación de las opciones de visualización
# Consultar la ayuda de "layout" para más información
fig <- fig %>% layout(
  scene = list(
    camera=list(
      eye = list(x=1.87, y=1.88, z=-1.64)
    )
  )
)
# Visualización del objeto interactivo
fig

# Tercer gráfico de ejemplo
# Este código grafica las curvas de nivel de la superficie

require(grDevices); require(graphics)
#filled.contour(x = seq(-10,10,length=60), y = seq(-9,9,length=60), z, color.palette = grey.colors, asp = 1)
filled.contour(x = seq(-10,10,length=60), y = seq(-9,9,length=60), z, color.palette = grey.colors)
title(main = "bivariate normal distribution")

######################################################################

# EJEMPLO 3

#Definimos la malla cuadrada donde representaremos la normal bivariada
#x<-seq(-4,8,length=60);y<-seq(-2,10,length=60)
x<-seq(-12,12,length=60);y<-x

# Función Normal Bivariada

f<-function(x,y,mu=c(2,4),s=c(3,-9,6))
{
  mu1<-mu[1];mu2<-mu[2]
  s11<-s[1];s12<-s[2];s22<-s[3];rho<-s12/(s11*s22)
  term1<-1/(2*pi*sqrt(s11*s22*(1-rho^2)))
  term2<--1/(2*(1-rho^2))
  term3<-(x-mu1)^2/s11
  term4<-(y-mu2)^2/s22
  term5<--2*rho*((x-mu1)*(y-mu2))/(sqrt(s11)*sqrt(s22))
  term1*exp(term2*(term3+term4-term5))
} 

z<-outer(x,y,f,s=c(3,-9,6)) # matriz de densidades
# Primer gráfico de ejemplo
# Este código grafica la superficie dada por la densidad de la norma bivariada
persp(x, y, z,col="lightgrey",theta=30, phi=20,r=50,d=0.1,expand=0.5,ltheta=90,
      main="bivariate normal distribution",lphi=180,shade=0.3,ticktype="detailed",
      nticks=5,cex.axis=0.7,zlab = 'f(x,y)', lwd = 0.05)
mtext(expression(list(mu[x]==2,mu[y]==4,sigma[x]==3,sigma[y]==6,
                      sigma[xy]==-9,rho==-0.5)), side=3)


# Segundo gráfico ejemplo
# Este código genera el objeto interactivo
fig <- plot_ly(z = ~z) %>% add_surface(
  contours = list(
    z = list(
      show=TRUE,
      usecolormap=TRUE,
      highlightcolor="#ff0000",
      project=list(z=TRUE)
    )
  )
)
# Modificación de las opciones de visualización
# Consultar la ayuda de "layout" para más información
fig <- fig %>% layout(
  scene = list(
    camera=list(
      eye = list(x=1.87, y=1.88, z=-1.64)
    )
  )
)
# Visualización del objeto interactivo
fig

# Tercer gráfico de ejemplo
# Este código grafica las curvas de nivel de la superficie

require(grDevices); require(graphics)
filled.contour(x = seq(-12,12,length=60), y = seq(-12,12,length=60), z, color.palette = grey.colors, asp = 1)
title(main = "bivariate normal distribution")








# Instalación del paquete desde un repositorio en caso de no estar instalado
# install.packages("plotly")
# Carga del paquete "plotly" si está instalado
library(plotly)



# EJEMPLO 2
#Definimos la malla cuadrada donde representaremos la normal bivariada
x<-seq(-3,9,length=60);y<-seq(1,13,length=60)


# Función Normal Bivariada
# Ejemplo 2:
f<-function(x,y,mu=c(3,7),s=c(3,12,6))
{
  mu1<-mu[1];mu2<-mu[2]
  s11<-s[1];s12<-s[2];s22<-s[3];rho<-s12/(s11*s22)
  term1<-1/(2*pi*sqrt(s11*s22*(1-rho^2)))
  term2<--1/(2*(1-rho^2))
  term3<-(x-mu1)^2/s11
  term4<-(y-mu2)^2/s22
  term5<--2*rho*((x-mu1)*(y-mu2))/(sqrt(s11)*sqrt(s22))
  term1*exp(term2*(term3+term4-term5))
} 

z<-outer(x,y,f,s=c(3,12,6)) # matriz de densidades
# Primer gráfico de ejemplo
# Este código grafica la superficie dada por la densidad de la norma bivariada
persp(x, y, z,col="lightgrey",theta=30, phi=20,r=50,d=0.1,expand=0.5,ltheta=90,
      main="bivariate normal distribution",lphi=180,shade=0.3,ticktype="detailed",
      nticks=5,cex.axis=0.7,zlab = 'f(x,y)', lwd = 0.05)
mtext(expression(list(mu[x]==3,mu[y]==7,sigma[x]==3,sigma[y]==6,
                      sigma[xy]==12,rho==0.5)), side=3)

# Segundo gráfico ejemplo
# Este código genera el objeto interactivo
fig <- plot_ly(z = ~z) %>% add_surface(
  contours = list(
    z = list(
      show=TRUE,
      usecolormap=TRUE,
      highlightcolor="#ff0000",
      project=list(z=TRUE)
    )
  )
)
# Modificación de las opciones de visualización
# Consultar la ayuda de "layout" para más información
fig <- fig %>% layout(
  scene = list(
    camera=list(
      eye = list(x=1.87, y=1.88, z=-1.64)
    )
  )
)
# Visualización del objeto interactivo
fig


# Tercer gráfico de ejemplo
# Este código grafica las curvas de nivel de la superficie

require(grDevices); require(graphics)
filled.contour(x = seq(-3,9,length=60), y = seq(1,13,length=60), z, color.palette = grey.colors, asp = 1)
title(main = "bivariate normal distribution")



# EJEMPLO 3

# Función Normal Bivariada
# Ejemplo 3:
f<-function(x,y,mu=c(3,7),s=c(3,15,6))
{
  mu1<-mu[1];mu2<-mu[2]
  s11<-s[1];s12<-s[2];s22<-s[3];rho<-s12/(s11*s22)
  term1<-1/(2*pi*sqrt(s11*s22*(1-rho^2)))
  term2<--1/(2*(1-rho^2))
  term3<-(x-mu1)^2/s11
  term4<-(y-mu2)^2/s22
  term5<--2*rho*((x-mu1)*(y-mu2))/(sqrt(s11)*sqrt(s22))
  term1*exp(term2*(term3+term4-term5))
} 

z<-outer(x,y,f,s=c(3,15,6)) # matriz de densidades
# Primer gráfico de ejemplo
# Este código grafica la superficie dada por la densidad de la norma bivariada
persp(x, y, z,col="lightgrey",theta=30, phi=20,r=50,d=0.1,expand=0.5,ltheta=90,
      main="bivariate normal distribution",lphi=180,shade=0.3,ticktype="detailed",
      nticks=5,cex.axis=0.7,zlab = 'f(x,y)', lwd = 0.05)
mtext(expression(list(mu[x]==3,mu[y]==7,sigma[x]==3,sigma[y]==6,
                      sigma[xy]==-12,rho==-0.5)), side=3)

# Segundo gráfico ejemplo
# Este código genera el objeto interactivo
fig <- plot_ly(z = ~z) %>% add_surface(
  contours = list(
    z = list(
      show=TRUE,
      usecolormap=TRUE,
      highlightcolor="#ff0000",
      project=list(z=TRUE)
    )
  )
)
# Modificación de las opciones de visualización
# Consultar la ayuda de "layout" para más información
fig <- fig %>% layout(
  scene = list(
    camera=list(
      eye = list(x=1.87, y=1.88, z=-1.64)
    )
  )
)
# Visualización del objeto interactivo
fig


# Tercer gráfico de ejemplo
# Este código grafica las curvas de nivel de la superficie

require(grDevices); require(graphics)
filled.contour(x = seq(-3,9,length=60), y = seq(1,13,length=60), z, color.palette = grey.colors, asp = 1)
title(main = "bivariate normal distribution")



