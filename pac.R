## Source: http://www.ncbi.nlm.nih.gov/pubmed/20463205
## J Neurophysiol. 2010 Aug;104(2):1195-210. doi: 10.1152/jn.00106.2010. Epub 2010 May 12.
## Measuring phase-amplitude coupling between neuronal oscillations of different frequencies.
## Tort AB1, Komorowski R, Eichenbaum H, Kopell N.

## Dependencies: signal, seewave, stats
library( "signal" )
library( "seewave" )

## TODO: Make globales into a R object with proper components.
## TODO: Wrap up in a "Phase Amplitude Coupling" R package.

## Synthetic signal model (from article)
pac.signal <- function( t, fp, fa, k, afp, afa ){
	0*afa*rnorm(length(t))/5 + 
	afa*sin(2*pi*fa*t)*((1-k)*sin(2*pi*fp*t)+1+k)/2 + 
	afp*sin(2*pi*fp*t)
}

pac.modular_index <- function( t, raw, fp, fa ){
	## Bandpass filter
	bfa = butter( 1, c(0.98*fa,1.02*fa), type=c("pass") )
	bfp = butter( 1, c(0.98*fp,1.02*fp), type=c("pass") )
	xfa <- filter( bfa, raw )
	xfp <- filter( bfp, raw )

	## Hilbert transforms (from the seewave package)
	phixp <- ifreq( xfp, length(t), plot=FALSE, phase=TRUE )
	ampfa <- env( xfa, length(t), plot=FALSE, ssmooth=100, envt="hil" )

	## Binning data on x_p phase values with bin size 1/10 radians
	brks <- seq( -pi, pi, by=0.1 )
	bs <- list( phi=phixp$p[,2], afa=ampfa )

	bs.classes <- cut( bs$phi, brks, include.lowest=TRUE )
	bs.means <- tapply( bs$afa, bs.classes, mean )
	bs.norm <- sum( bs.means )

	## Now ready to compute the proposed Modulation Index (MI)
	bs.p <- bs.means/bs.norm
	bs.MI <- (log(length(bs.p))+sum(bs.p*log(bs.p)))/log(length(bs.p))
	return( bs.MI )
}

## Example
f_p <- 0.0025
f_a <- 0.41
chi <- 2
Abar_fa = 1
Abar_fp = 4

t = seq( 0, 1000, by=1 )

bfa = butter( 1, c(0.98*f_a,1.02*f_a), type=c("pass") )
bfp = butter( 1, c(0.98*f_p,1.02*f_p), type=c("pass") )

## Synthetic raw signal
xraw = pac.signal( t, f_p, f_a, chi, Abar_fp , Abar_fa  )

## Filter at frequency ranges of interest f_A and f_p
x_fa <- filter( bfa, xraw )
x_fp <- filter( bfp, xraw )

## Hilbert transforms (from the seewave package)
phi_xp <- ifreq( x_fp, 1000, plot=FALSE, phase=TRUE )
## For amplitudes we use the reconstructed envelope and the original for comparison
amp_fa <- env( x_fa, 1000, plot=FALSE, ssmooth=100, envt="hil" )
amp_fa_original <- ((1-chi)*sin(2*pi*f_p*t)+1+chi)/2

## Binning data on x_p phase values with bin size 1/10 radians
breaks <- seq( -pi, pi, by=0.1 )
bins <- list( phi=phi_xp$p[,2], afa=amp_fa, afa_original=amp_fa_original )
bins.classes <- cut( bins$phi, breaks, include.lowest=TRUE )
bins.means <- tapply( bins$afa, bins.classes, mean )
bins.means_original <- tapply( bins$afa_original, bins.classes, mean )
bins.norm <- sum( bins.means )
bins.norm_original <- sum( bins.means_original )

## Plots
par( mfrow=c(3,2) )
plot( t, xraw, type="l", main="Raw Signal (x_raw)", ylab="mV" )
plot( t, x_fa, col="blue", type="l", main="LG Filtered Signals (x_fa)", ylab="mV" )
plot( t, x_fp, col="blue", type="l", main="Theta Filtered Signals (x_fp)", ylab="mV" )

## Time series of phases of x_fp(t) is returned by ifreq which applies Hilbert transform
## from the seewave package
ifreq( x_fp, 1000, plot=TRUE, phase=TRUE, xlab="t" )
title( main="Theta Phases (Phi_fp)" )

# Time series of the amplitude envelope of x_fa is returned by env, also via the Hilbert transform
## from the seewave package
env( x_fa, 1000, plot=TRUE, envt="hil", ssmooth=100, alab="mV", tlab="t", cexlab=0.75 )
lines( t, ((1-chi)*sin(2*pi*f_p*t)+1+chi)/2, type="l", col="blue" )
title( main="LG Amplitude (A_fa)", sub="Hilbert transform (black) v. Original (blue)" )

## Phase-Amplitude plots
plot( bins.means/bins.norm, type="s", xlab="Theta phase (bin #)", ylab="LG Amplitude", 
	main="Phase-Amplitude", sub="Hilbert transform (black) v. Original (blue)" )
lines( bins.means_original/bins.norm_original, type="s", col="blue" ) 

## Now ready to compute the proposed Modulation Index (MI)
bins.p <- bins.means_original/bins.norm_original
bins.MI_o <- (log(length(bins.p))+sum(bins.p*log(bins.p)))/log(length(bins.p))
print( bins.MI_o )
bins.p <- bins.means/bins.norm
bins.MI <- (log(length(bins.p))+sum(bins.p*log(bins.p)))/log(length(bins.p))
print( bins.MI )

## Heat map

fas <- seq(0.001, 0.005, by=0.0001 )
fps <- seq(0.1, 0.5, by=0.01 )

bins.mimatrix <- matrix( data=NA, nrow=length(fps), ncol=length(fas) )
for( j in 1:length(fas) ){
	for( i in 1:length(fps) ){
		bins.mimatrix[i,j]=pac.modular_index( t, xraw, fps[i], fas[j] )
	}
}
dev.new()
heatmap( bins.mimatrix, Rowv=NA, Colv=NA, 
	labRow=fps, labCol=fas, xlab="Fp", ylab="Fa" )





