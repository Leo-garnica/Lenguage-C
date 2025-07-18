#include<stdio.h>

void main()
{
    int bt[20],p[20],at[20],wt[20],tat[20],i,j,n,total=0,pos,temp;
    float rt[20];
    float avg_wt, avg_rt;

    printf("Entrar procesos:");
    scanf("%d",&n);

    printf("\nIngresar Tiempo de Llegada y Tiempo de Servicio:\n");
    for(i=0;i<n;i++)
    {
        printf("p%d-Tiempo llegada:",i+1);
        scanf("%d",&at[i]);
        printf("p%d-Tiempo servicio:",i+1);
        scanf("%d",&bt[i]);
        p[i]=i+1;           //contiene número de procesos
    }

    //ordenando tiempo de servicio en orden ascedente
    for(i=0;i<n;i++)
    {
        pos=i;
        for(j=i+1;j<n;j++)
        {
            if(at[j]<at[pos] || (at[j]==at[pos] && bt[j]<bt[pos]))
                pos=j;
        }

        temp=at[i];
        at[i]=at[pos];
        at[pos]=temp;

        temp=bt[i];
        bt[i]=bt[pos];
        bt[pos]=temp;

        temp=p[i];
        p[i]=p[pos];
        p[pos]=temp;
    }

    int completados[20] = {0};
    int tempAct = 0;
    int cantCompletados = 0;

    while(cantCompletados < n)
    {
        int masCorto = -1;
        int menorServicio = 9999;

        for(i=0;i<n;i++)
        {
            if(!completados[i] && at[i] <= tempAct && bt[i] < menorServicio)
            {
                menorServicio = bt[i];
                masCorto = i;
            }
        }

        if(masCorto == -1)
        {
            tempAct++;
        }
        else
        {
            wt[masCorto] = tempAct - at[masCorto];
            tat[masCorto] = wt[masCorto] + bt[masCorto];
            rt[masCorto] = (float)tat[masCorto] / bt[masCorto];

            completados[masCorto] = 1;
            cantCompletados++;
            tempAct += bt[masCorto];
        }
    }

    total = 0;
    for(i=0;i<n;i++)
        total += wt[i];
    avg_wt=(float)total/n;      //promedio tiempo de espera

    float total_rt = 0;
    for(i=0;i<n;i++)
        total_rt += rt[i];
    avg_rt = total_rt/n;

    printf("\nProceso | Tiempo de llegada | Tiempo de servicio | Tiempo de Espera | Tiempo de Retorno | Tiempo de Respuesta");
    for(i=0;i<n;i++)
    {
        printf("\np%d\t\t%d\t\t    %d\t\t\t%d\t\t    %d\t\t\t%.2f",
            p[i],at[i],bt[i],wt[i],tat[i],rt[i]);
    }

    printf("\n\nPromedio Tiempo de Espera=%.2f",avg_wt);
    printf("\nPromedio Tiempo de Respuesta=%.2f\n",avg_rt);
}